DEPS = logicalFS.h volumeEntry.h entry.h fsLow.h

# Handle changes in the dependencies
%.o: %.c $(DEPS)
	gcc -c -o $@ $< -lm

# Run driver
fsdriver3: fsdriver3.c fsLow.c logicalFS.c
	gcc fsdriver3.c fsLow.c logicalFS.c -o fsdriver3 -lm

# Compile hexdump
init_hexdump:
	gcc hexdump.c -o hexdump

# Run hexdump
hex_dump: 
	./hexdump OurVolume --start $(START) --count $(COUNT)

# Remove executables
# Need to remove --> volume?, (*.o) [currently hardcoded]
clean:
	rm -f fsdriver3 hexdump
