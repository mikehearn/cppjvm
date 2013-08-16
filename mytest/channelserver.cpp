// Notes:
//  - Compile until you hit the unit tests which are expected to fail (took out the classes they need)
//  - ExtensionRegistry won't compile due to static method return type covariance. just delete the lite versions of the overloaded methods.
//  - Command below produces a working executable. will require special rpath to be bundled into a .app
//
// g++ -I../java/jniutil/include/ -I../java/cppjvm/gen/include -I../thirdparty/jvm/darwin/include ../java/jniutil/lib/libjniutil.lib -L /Library/Java/JavaVirtualMachines/jdk1.7.0_15.jdk/Contents/Home/jre/lib/server -ljvm ../java/cppjvm/lib/libcppjvm.lib -Wl,-rpath -Wl,/Library/Java/JavaVirtualMachines/jdk1.7.0_15.jdk/Contents/Home/jre/lib/server bcj-hello-world.cpp native-listeners.cpp && ./a.out
// 

#include <unistd.h>

#include <jvm/virtual_machine.hpp>
#include <jvm/local_frame.hpp>

#include <iostream>
#include <string>
#include <java/lang/System.hpp>

#include <com/google/bitcoin/core/Utils.hpp>
#include <com/google/bitcoin/utils/BriefLogFormatter.hpp>
#include <com/google/bitcoin/kits/WalletAppKit.hpp>
#include <com/google/bitcoin/protocols/channels/PaymentChannelServerListener.hpp>
#include <com/google/bitcoin/protocols/channels/PaymentChannelCloseException/CloseReason.hpp>

#include "native-listeners.h"

using namespace com::google::bitcoin::core;
using namespace com::google::bitcoin::protocols::channels;
using namespace com::google;
using namespace jvm;

class ConnectionEventHandler : public native::PaymentChannelServerConnectionEventHandler {
	virtual void channelOpen(Sha256Hash channelId) {
		fprintf(stderr, "channelOpen\n");
	};
	virtual void paymentIncrease(java::math::BigInteger by, java::math::BigInteger to) {
		fprintf(stderr, "paymentIncrease by %ld to %ld\n", by.longValue(), to.longValue());
	};
	virtual void channelClosed(com::google::bitcoin::protocols::channels::PaymentChannelCloseException_n::CloseReason reason) {
		fprintf(stderr, "channelClosed %s\n", reason.name().str().c_str());
	};
};

class HandlerFactory : public native::PaymentChannelHandlerFactory {
public:
	virtual native::PaymentChannelServerConnectionEventHandler* onNewConnection(java::net::SocketAddress clientAddress) {
		return new ConnectionEventHandler();
	}
};

int main(int argc, char *argv[])
{
	try
	{
		create_global_vm("bitcoinj-tools-0.11-SNAPSHOT.jar");
	}
	catch (const std::exception &e)
	{
		std::cout << "Initialising JVM: " << e.what() << std::endl;
	}

	// Any variables declared that aren't jvm::global<> whilst this is in scope will become
	// eligible for garbage collection after it goes out of scope. Here we just cover the
	// whole main function.
	local_frame lf;

	// Register native callback methods (see native-listeners.cpp)
	register_natives();

	// Simple example of making a Java static method call.
	std::string vm_name = java::lang::System::getProperty("java.vm.name");
	std::cout << vm_name << std::endl;

	// Make the bitcoinj logging more concise.
	bitcoin::utils::BriefLogFormatter::init();

	// Create a basic wallet app.
	NetworkParameters params = NetworkParameters::regTests();

	java::io::File directory(".");
	bitcoin::kits::WalletAppKit kit(params, directory, "hello-world");
	// Connect to localhost
	jvm::array<PeerAddress> addresses(4);
	addresses.put(0, new PeerAddress(java::net::InetSocketAddress("localhost", 1).getAddress(), 18444));
	addresses.put(1, new PeerAddress(java::net::InetSocketAddress("localhost", 1).getAddress(), 18335));
	addresses.put(2, new PeerAddress(java::net::InetSocketAddress("localhost", 1).getAddress(), 18336));
	addresses.put(3, new PeerAddress(java::net::InetSocketAddress("localhost", 1).getAddress(), 18447));
	kit.setPeerNodes(addresses);
	// Wait for the network to come up and the block chain to synchronize.
	kit.startAndWait();

	// Kick off a java listener which accepts payment channels
	HandlerFactory* factory = new HandlerFactory();
	(new PaymentChannelServerListener(kit.peerGroup(), kit.wallet(), 15, Utils::get_CENT(), factory->peer))->bindAndStart(4242);

	// Just sit and wait. Program is immune to ctrl-c at this point :-(
	std::cout << "Please type something and hit enter to quit." << std::endl;
	std::string tmp;
	std::cin >> tmp;
	std::cout << "Quitting ..." << std::endl;
	_exit(0);
}
