
all:
	echo "This makefile is for cleaning only."

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C ksdk clean
	$(MAKE) -C user/kprogram clean
