# Nome do executável
TARGET = ftp_client

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# Arquivos fonte
SRCS = ftp.c aux.c
OBJS = $(SRCS:.c=.o)

# Regras principais
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos compilados
clean:
	rm -f $(OBJS) $(TARGET)

# Regra para recompilar o projeto do zero
rebuild: clean all

# Declara arquivos que não devem ser tratados como alvo
.PHONY: all clean rebuild
