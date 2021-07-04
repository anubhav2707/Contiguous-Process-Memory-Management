all: MemMangement

MemMangement:
	gcc -g -Wall -o holes MemManage.c -lm
clean:
	$(RM) holes
