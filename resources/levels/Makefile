DB ?= ../offline.db
MAPS = $(wildcard *.map)

# DB inherited from top-level makefile
all: $(DB)

$(DB): $(MAPS)
	sqlite3 $(DB) "DELETE FROM levels"
	for i in `ls *.map | sed "s/'/\\'/g"`; \
	do \
		sqlite3 $(DB) "INSERT INTO levels (spec) VALUES ('$$(cat $$i | sed "s#'#''#g")')"; \
	done
