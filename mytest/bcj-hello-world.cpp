// Notes:
//  - Compile until you hit the unit tests which are expected to fail (took out the classes they need)
//  - ExtensionRegistry won't compile due to static method return type covariance. just delete the lite versions of the overloaded methods.
//  - Command below produces a working executable. will require special rpath to be bundled into a .app
//
// Mac:
// g++ -I../java/jniutil/include/ -I../java/cppjvm/gen/include -I../thirdparty/jvm/darwin/include ../java/jniutil/lib/libjniutil.lib -L /Library/Java/JavaVirtualMachines/jdk1.7.0_15.jdk/Contents/Home/jre/lib/server -ljvm ../java/cppjvm/lib/libcppjvm.lib -Wl,-rpath -Wl,/Library/Java/JavaVirtualMachines/jdk1.7.0_15.jdk/Contents/Home/jre/lib/server bcj-hello-world.cpp native-listeners.cpp && ./a.out
//
// Linux:
// g++ -g -I../java/jniutil/include/ -I../java/cppjvm/gen/include -I../thirdparty/jvm/linux/include bcj-hello-world.cpp native-listeners.cpp ../java/jniutil/lib/libjniutil.lib ../java/cppjvm/lib/libcppjvm.lib -L$JVM_DIR -ljvm && LD_LIBRARY_PATH=$JVM_DIR ./a.out
// 

#include <unistd.h>

#include <jvm/virtual_machine.hpp>
#include <jvm/local_frame.hpp>

#include <iostream>
#include <string>
#include <java/lang/System.hpp>

#include <com/google/bitcoin/utils/BriefLogFormatter.hpp>
#include <com/google/bitcoin/kits/WalletAppKit.hpp>
#include <com/google/bitcoin/jni/NativeWalletEventListener.hpp>

#include "native-listeners.h"

using namespace com::google::bitcoin::core;
using namespace com::google;
using namespace jvm;

class ExampleBalanceListener : public native::WalletEventListener {
public:
	virtual void onCoinsReceived(Wallet &wallet, Transaction &tx, long prevBalance, long newBalance) {
		// Callbacks run on the user thread (see bitcoinj documentation)
		std::cout << "We got some moneyzzz!! Received " << (newBalance - prevBalance) << " satoshis" << std::endl;
	}

	virtual void onKeysAdded(Wallet &wallet, java::util::List &keys) {
		// Callbacks run on the user thread (see bitcoinj documentation)
		std::cout << "Someone added " << keys.size() << " keys" << std::endl;
	}
};

int main(int argc, char *argv[])
{
	try
	{
		// This has to be the "tools" JAR for now because we need a bundled Jar that has
		// everything, to avoid a really annoying and long classpath.
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
	NetworkParameters params = NetworkParameters::testNet();

	java::io::File directory(".");
	bitcoin::kits::WalletAppKit kit(params, directory, "hello-world");
	// Wait for the network to come up and the block chain to synchronize.
	kit.startAndWait();
	// Register a listener to find out when we received coins.
	ExampleBalanceListener *listener = new ExampleBalanceListener();
	kit.wallet().addEventListener(listener->peer);
	// Create a new key.
	bitcoin::core::ECKey key = jnew<bitcoin::core::ECKey>();
	kit.wallet().addKey(key);

	std::string address = key.toAddress(params).toString();
	std::cout << "Plz send some moneyz to " << address << std::endl;

	// Just sit and wait. Program is immune to ctrl-c at this point :-(
	std::cout << "Please type something and hit enter to quit.";
	std::string tmp;
	std::cin >> tmp;

	// Must force process to go away without waiting for JVM due to stupid
	// netty non-daemon thread (netty will go away soon).
	std::cout << "Quitting ...";
	_exit(0);
}
