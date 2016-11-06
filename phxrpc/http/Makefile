
include ../../phxrpc.mk

TEST_TARGETS = test_http_client test_file_util

all: $(TEST_TARGETS)

test_http_client: test_http_client.o
	$(LINKER) $^ -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) -g2 -o $@
test_file_util: test_file_util.o
	$(LINKER) $^ -L$(PHXRPC_ROOT)/lib -lphxrpc $(LDFLAGS) -g2 -o $@
clean:
	@( $(RM) $(TEST_TARGETS) )
	@( $(RM) *.o core.* $(LIB_OBJS) )
