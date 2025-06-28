.PHONY: all clean part1 part2 part3 part4 part5 part6 part7 part8 part9 part10

all: part1 part2 part3 part4 part5 part6 part7 part8 part9 part10

part1:
	$(MAKE) -C Part_1

part2:
	$(MAKE) -C Part_2

part3:
	$(MAKE) -C Part_3

part4:
	$(MAKE) -C Part_4

part5:
	$(MAKE) -C Part_5

part6:
	$(MAKE) -C Part_6

part7:
	$(MAKE) -C Part_7

part8:
	$(MAKE) -C Part_8

part9:
	$(MAKE) -C Part_9

part10:
	$(MAKE) -C Part_10

clean:
	$(MAKE) -C Part_1 clean
	$(MAKE) -C Part_2 clean
	$(MAKE) -C Part_3 clean
	$(MAKE) -C Part_4 clean
	$(MAKE) -C Part_5 clean
	$(MAKE) -C Part_6 clean
	$(MAKE) -C Part_7 clean
	$(MAKE) -C Part_8 clean
	$(MAKE) -C Part_9 clean
	$(MAKE) -C Part_10 clean