CFLAGS = -g -Wall -Wextra -O3
LDFLAGS = -pthread -lm # Linker flags for pthreads



all: website_binary_ttable tests

tests: evaluation.o binary_converter.o stack.o table_builders.o shunting_yard.o find_nr_of_vars.o int_stack.o tests.o
	@gcc $(CFLAGS) evaluation.o binary_converter.o stack.o table_builders.o shunting_yard.o find_nr_of_vars.o int_stack.o tests.o -o tests $(LDFLAGS) -lcunit
	@chmod +x tests

tests.o: tests.c
	@echo "Compiling tests"
	@gcc $(CFLAGS) -c tests.c

website_binary_ttable: evaluation.o binary_converter.o stack.o table_builders.o website_main.o shunting_yard.o find_nr_of_vars.o int_stack.o
	@echo "linking and producing the final application"
	@gcc $(CFLAGS) evaluation.o binary_converter.o stack.o table_builders.o website_main.o shunting_yard.o find_nr_of_vars.o int_stack.o -o website_binary_ttable $(LDFLAGS)
	@chmod +x website_binary_ttable

evaluation.o: rpn_evaluator/evaluation.c
	@echo "Compiling evaluation"
	@gcc $(CFLAGS) -c rpn_evaluator/evaluation.c

binary_converter.o: converters/binary_converter.c 
	@echo "Compiling binary_converter"
	@gcc $(CFLAGS) -c converters/binary_converter.c 

stack.o: data_structs/stack.c
	@echo "Compiling stack"
	@gcc $(CFLAGS) -c data_structs/stack.c

int_stack.o: data_structs/int_stack.c 
	@echo "Compiling int stack"
	@gcc $(CFLAGS) -c data_structs/int_stack.c 

shunting_yard.o: converters/shunting_yard.c 
	@echo "Compiling shunting yard conversion"
	@gcc $(CFLAGS) -c converters/shunting_yard.c

find_nr_of_vars.o: utils/find_nr_of_vars.c 
	@echo "Compiling find_nr_of_vars"
	@gcc $(CFLAGS) -c utils/find_nr_of_vars.c


table_builders.o: table_builders_for_webpage/table_builders.c
	@echo "Compiling generate_table_direct_to_file"
	@gcc $(CFLAGS) -c table_builders_for_webpage/table_builders.c

website_main.o : website_main.c 
	@echo "Compiling website_main"
	@gcc $(CFLAGS) -c website_main.c

clean:
	@echo "removing files"
	@rm table_builders.o evaluation.o stack.o website_main.o shunting_yard.o binary_converter.o find_nr_of_vars.o int_stack.o tests.o
