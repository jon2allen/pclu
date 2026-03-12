# Makefile for ~CLU/code/libclu/Opt

CLU	= $(CLUHOME)/exe/pclu
LIB_DIR	= $(CLUHOME)/lib
LIBS 	= misc lowlev useful
LIB_CLU = $(CLUHOME)/lib/clu

RM      = /bin/rm -f
FLUFF   = *~ *.old *.bak .*~ .*.old .*.bak

o_files: ${OBJS}

c_files:
	cd ${LIB_DIR}; $(MAKE) CLUHOME=$(CLUHOME) libs
	$(CLU)  \#bindir `pwd` \#ext false \#dir ${LIB_DIR} \#ce gcd.equ \#me ${LIBS} \
		\#spec _host_address.spc _all_dir_entries_.spc \
		\#dir ${LIB_CLU} 		\
		\#spec gc_read \
		\#opt time \#cf \#co \*

clean:
	${RM} *.o ${FLUFF}

veryclean: clean
