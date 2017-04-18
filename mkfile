</$objtype/mkfile

LIB=$objtype/lib/libfibheap.a

OFILES=\
	fibheap.$O\

</sys/src/cmd/mksyslib

tests:V: fibheap.$O
	cd test
	mk all

cleantests:V:
	cd test
	mk clean
