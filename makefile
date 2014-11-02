program_NAME := dina
program_SRCS := data.c dispose.c fileInstruction.c global.c  instruction_Check.c instruction_Handle.c main.c phase1.c phase2.c stringops.c symbolManage.c\

program_OBJS := ${program_SRCS:.c=.o}
clean_list += $(program_OBJS) $(program_NAME) *.ob *.ext *.ent

# C Preprocessor Flags
CPPFLAGS +=
# compiler flags for gccs
CFLAGS += -ansi -Wall -Wextra -pedantic-errors
# some flags to make things easier on ValGrind, add to CFLAGS
VGFLAGS := -g -fno-inline-functions -fno-inline-functions-called-once \
	-fno-optimize-sibling-calls

.PHONY: clean indent

$(program_NAME): $(program_OBJS)
	@ $(LINK.c) $(program_SRCS) -o $@

Test: $(program_NAME)
	./$(program_NAME) ps.as 

clean:
	@- $(RM) $(clean_list)

indent_SRCS := $(program_SRCS)
indent:
	@ indent -linux -brf -prs $(indent_SRCS)

# Generate dependencies for all files in project
%.d: $(program_SRCS)
	@ $(CC) $(CPPFLAGS) -MM $*.c | sed -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

clean_list += ${program_SRCS:.c=.d}

# Include the list of dependancies generated for each object file
# unless make was called with target clean
ifneq "$(MAKECMDGOALS)" "clean"
-include ${program_SRCS:.c=.d}
endif
