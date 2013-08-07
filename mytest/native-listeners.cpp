// Defines a C function for each native method in NativeWalletEventListener (and others in future).
// They have to be marked JNIEXPORT and appear in an exports table at the bottom so the JVM can find them.

#include "native-listeners.h"

#include <iostream>

#include "jni.h"

#include <jvm/local_frame.hpp>
#include <jvm/object.hpp>
#include <java/math/BigInteger.hpp>
#include <java/util/AbstractList.hpp>
#include <com/google/bitcoin/core/Wallet.hpp>
#include <com/google/bitcoin/core/Transaction.hpp>
#include <com/google/bitcoin/jni/NativeWalletEventListener.hpp>
#include <com/google/bitcoin/jni/NativePaymentChannelServerConnectionEventHandler.hpp>
#include <com/google/bitcoin/jni/NativePaymentChannelHandlerFactory.hpp>

using namespace com::google::bitcoin::core;
using namespace com::google::bitcoin::jni;

extern "C" {

void JNICALL Wallet_onCoinsReceived(JNIEnv *env, jobject self, jobject wallet, jobject tx, jobject prevBalance, jobject newBalance) {
    std::cout << "onCoinsReceived" << std::endl;
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativeWalletEventListener self_(self);
    Wallet wallet_(wallet);
    Transaction tx_(tx);
    long prevBalance_ = java::math::BigInteger(prevBalance).longValue();
    long newBalance_ = java::math::BigInteger(newBalance).longValue();

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::WalletEventListener *listener = (native::WalletEventListener*)self_.get_ptr();
    listener->onCoinsReceived(wallet_, tx_, prevBalance_, newBalance_);
}

void JNICALL Wallet_onCoinsSent(JNIEnv *, jobject, jobject, jobject, jobject, jobject) {
    std::cout << "onCoinsSent" << std::endl;
}

void JNICALL Wallet_onReorganize(JNIEnv *, jobject, jobject) {
    std::cout << "onReorganize" << std::endl;
}

void JNICALL Wallet_onTransactionConfidenceChanged(JNIEnv *, jobject, jobject, jobject) {

}

void JNICALL Wallet_onWalletChanged(JNIEnv *, jobject, jobject) {
    std::cout << "onWalletChanged" << std::endl;
}

// self, Wallet, List<ECKey>
void JNICALL Wallet_onKeysAdded(JNIEnv *env, jobject self, jobject wallet, jobject keys) {
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativeWalletEventListener self_(self);
    Wallet wallet_(wallet);
    java::util::List keys_(keys);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::WalletEventListener *listener = (native::WalletEventListener*)self_.get_ptr();
    listener->onKeysAdded(wallet_, keys_);
}


void JNICALL ChannelServerListener_channelOpen(JNIEnv *env, jobject self, jobject channelId) {
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativePaymentChannelServerConnectionEventHandler self_(self);
    com::google::bitcoin::core::Sha256Hash channelId_(channelId);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::PaymentChannelServerConnectionEventHandler *listener = (native::PaymentChannelServerConnectionEventHandler*)self_.get_ptr();
    listener->channelOpen(channelId_);
}

void JNICALL ChannelServerListener_paymentIncrease(JNIEnv *env, jobject self, jobject by, jobject to) {
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativePaymentChannelServerConnectionEventHandler self_(self);
    java::math::BigInteger by_(by);
    java::math::BigInteger to_(to);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::PaymentChannelServerConnectionEventHandler *listener = (native::PaymentChannelServerConnectionEventHandler*)self_.get_ptr();
    listener->paymentIncrease(by_, to_);
}

void JNICALL ChannelServerListener_channelClosed(JNIEnv *env, jobject self, jobject reason) {
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativePaymentChannelServerConnectionEventHandler self_(self);
    com::google::bitcoin::protocols::channels::PaymentChannelCloseException_n::CloseReason reason_(reason);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::PaymentChannelServerConnectionEventHandler *listener = (native::PaymentChannelServerConnectionEventHandler*)self_.get_ptr();
    listener->channelClosed(reason_);
}


jvm::object JNICALL ChannelServerHandlerFactory_onNewConnection(JNIEnv *env, jobject self, jobject clientAddress) {
    jvm::local_frame lf(16, env);  // Ensure things we reference get garbage collected.
    // Wrap with CPPJVM wrapper objects to make them more C++like.
    NativePaymentChannelHandlerFactory self_(self);
    java::net::SocketAddress clientAddress_(clientAddress);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::PaymentChannelHandlerFactory *listener = (native::PaymentChannelHandlerFactory*)self_.get_ptr();
    native::PaymentChannelServerConnectionEventHandler* handler = listener->onNewConnection(clientAddress_);
    if (handler == NULL)
        return NativePaymentChannelHandlerFactory();
    else
        return handler->peer;
}

}  // extern C

// These signature strings in the middle can be found using the javap tool in the JDK:
//
// javap -s com/google/bitcoin/jni/NativeWalletEventListener.class
static JNINativeMethod wallet_event_listener_registrations[] = {
    {(char* const)"onWalletChanged", (char* const)"(Lcom/google/bitcoin/core/Wallet;)V", (void *)&Wallet_onWalletChanged},
    {(char* const)"onKeysAdded", (char* const)"(Lcom/google/bitcoin/core/Wallet;Ljava/util/List;)V", (void *)&Wallet_onKeysAdded},
    {(char* const)"onTransactionConfidenceChanged", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;)V", (void *)&Wallet_onTransactionConfidenceChanged},
    {(char* const)"onReorganize", (char* const)"(Lcom/google/bitcoin/core/Wallet;)V", (void *)&Wallet_onReorganize},
    {(char* const)"onCoinsSent", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;Ljava/math/BigInteger;Ljava/math/BigInteger;)V", (void *)&Wallet_onCoinsSent},
    {(char* const)"onCoinsReceived", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;Ljava/math/BigInteger;Ljava/math/BigInteger;)V", (void *)&Wallet_onCoinsReceived}
};

static JNINativeMethod channel_server_event_handler_registrations[] = {
    {(char* const)"channelOpen", (char* const)"(Lcom/google/bitcoin/core/Sha256Hash;)V", (void *)&ChannelServerListener_channelOpen},
    {(char* const)"paymentIncrease", (char* const)"(Ljava/math/BigInteger;Ljava/math/BigInteger;)V", (void *)&ChannelServerListener_paymentIncrease},
    {(char* const)"channelClosed", (char* const)"(Lcom/google/bitcoin/protocols/channels/PaymentChannelCloseException$CloseReason;)V", (void *)&ChannelServerListener_channelClosed},
};

static JNINativeMethod channel_server_event_handler_factory_registrations[] = {
    {(char* const)"onNewConnection", (char* const)"(Ljava/net/SocketAddress;)Lcom/google/bitcoin/protocols/channels/ServerConnectionEventHandler;", (void *)&ChannelServerHandlerFactory_onNewConnection},
};

void register_natives() {
    jclass wallet_event_listener = jvm::global_vm().env()->FindClass("com/google/bitcoin/jni/NativeWalletEventListener");
    jvm::global_vm().env()->RegisterNatives(wallet_event_listener, wallet_event_listener_registrations, 6);

    jclass channel_server_event_handler = jvm::global_vm().env()->FindClass("com/google/bitcoin/jni/NativePaymentChannelServerConnectionEventHandler");
    jvm::global_vm().env()->RegisterNatives(channel_server_event_handler, channel_server_event_handler_registrations, 3);

    jclass channel_server_event_handler_factory = jvm::global_vm().env()->FindClass("com/google/bitcoin/jni/NativePaymentChannelHandlerFactory");
    jvm::global_vm().env()->RegisterNatives(channel_server_event_handler_factory, channel_server_event_handler_factory_registrations, 1);
}
