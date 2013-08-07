// Define C++ equivalents for Java callback objects. Derive from them to handle events (usually, on a framework thread).
// A pointer to one of these is stuffed into a java-side class in the c.g.b.jni package and calls are relayed via JNI
// to here in native-listeners.cpp.

#ifndef NATIVE_LISTENERS_CPP
#define NATIVE_LISTENERS_CPP

#include <jvm/object.hpp>
#include <jvm/global.hpp>

#include <java/util/List.hpp>
#include <java/net/SocketAddress.hpp>
#include <com/google/bitcoin/core/Wallet.hpp>
#include <com/google/bitcoin/core/Transaction.hpp>
#include <com/google/bitcoin/protocols/channels/PaymentChannelCloseException.hpp>
#include <com/google/bitcoin/jni/NativeWalletEventListener.hpp>
#include <com/google/bitcoin/jni/NativePaymentChannelServerConnectionEventHandler.hpp>
#include <com/google/bitcoin/jni/NativePaymentChannelHandlerFactory.hpp>

using namespace com::google::bitcoin::core;
using namespace com::google::bitcoin::jni;

void register_natives();

// Let us avoid a naming conflict between the Java WalletEventListener and the C++ WalletEventListener.
namespace native {

class WalletEventListener {
public:
	jvm::global<NativeWalletEventListener> peer;

	// Constructs a new WalletEventListener with a Java-side peer, that can then be added to a Wallet object.
	WalletEventListener() {
		peer = jvm::jnew<NativeWalletEventListener>();
		peer.set_ptr((jlong)this);
	}

	virtual void onCoinsReceived(Wallet &wallet, Transaction &tx, long prevBalance, long newBalance) {};
	virtual void onKeysAdded(Wallet &wallet, java::util::List &keys) {};

	// ... etc ...
};

class PaymentChannelServerConnectionEventHandler {
public:
	jvm::global<NativePaymentChannelServerConnectionEventHandler> peer;

	// Constructs a new PaymentChannelServerListener with a Java-side peer, that can then be added to a Wallet object.
	PaymentChannelServerConnectionEventHandler() {
		peer = jvm::jnew<NativePaymentChannelServerConnectionEventHandler>();
		peer.set_ptr((jlong)this);
	}

	virtual void channelOpen(Sha256Hash channelId) {};
	virtual void paymentIncrease(java::math::BigInteger by, java::math::BigInteger to) {};
	virtual void channelClosed(com::google::bitcoin::protocols::channels::PaymentChannelCloseException_n::CloseReason reason) {};
};

class PaymentChannelHandlerFactory {
public:
	jvm::global<NativePaymentChannelHandlerFactory> peer;

	// Constructs a new PaymentChannelServerListener with a Java-side peer, that can then be added to a Wallet object.
	PaymentChannelHandlerFactory() {
		peer = jvm::jnew<NativePaymentChannelHandlerFactory>();
		peer.set_ptr((jlong)this);
	}

	virtual PaymentChannelServerConnectionEventHandler* onNewConnection(java::net::SocketAddress clientAddress) {};
};

}  // namespace native

#endif
