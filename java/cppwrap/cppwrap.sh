echo "Working" >/dev/tty;
java -cp java/cppwrap/jar/cppwrap.jar:bitcoinj-tools-0.10-SNAPSHOT.jar com.earwicker.cppjvm.cppwrap.CppWrap $@
