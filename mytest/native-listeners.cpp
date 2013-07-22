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
    // TODO: Huh? Why can't we use the interface here...
    java::util::AbstractList keys_(keys);

    // Must use a C style cast here as we're doing a cast from a "jlong" (i.e. long) to a C++ object pointer.
    native::WalletEventListener *listener = (native::WalletEventListener*)self_.get_ptr();
    listener->onKeysAdded(wallet_, keys_);
}

}  // extern C

// These signature strings in the middle can be found using the javap tool in the JDK:
//
// javap -s com/google/bitcoin/jni/NativeWalletEventListener.class
static JNINativeMethod registrations[] = {
    {(char* const)"onWalletChanged", (char* const)"(Lcom/google/bitcoin/core/Wallet;)V", (void *)&Wallet_onWalletChanged},
    {(char* const)"onKeysAdded", (char* const)"(Lcom/google/bitcoin/core/Wallet;Ljava/util/List;)V", (void *)&Wallet_onKeysAdded},
    {(char* const)"onTransactionConfidenceChanged", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;)V", (void *)&Wallet_onTransactionConfidenceChanged},
    {(char* const)"onReorganize", (char* const)"(Lcom/google/bitcoin/core/Wallet;)V", (void *)&Wallet_onReorganize},
    {(char* const)"onCoinsSent", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;Ljava/math/BigInteger;Ljava/math/BigInteger;)V", (void *)&Wallet_onCoinsSent},
    {(char* const)"onCoinsReceived", (char* const)"(Lcom/google/bitcoin/core/Wallet;Lcom/google/bitcoin/core/Transaction;Ljava/math/BigInteger;Ljava/math/BigInteger;)V", (void *)&Wallet_onCoinsReceived}
};

void register_natives() {
    jclass clazz = jvm::global_vm().env()->FindClass("com/google/bitcoin/jni/NativeWalletEventListener");
    jvm::global_vm().env()->RegisterNatives(clazz, registrations, 6);
}