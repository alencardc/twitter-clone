all: server client

server: 
	make -f base.mk TARGET_EXEC=app_server SRC_DIRS="src/server src/lib"
	@echo Server compiled successfully!

client:
	make -f base.mk TARGET_EXEC=app_client SRC_DIRS="src/client src/lib" EXTRA_LIBS="-lncurses"
	@echo Client compiled successfully!

clean:
	make -f base.mk clean
	@echo So fresh, so clean!