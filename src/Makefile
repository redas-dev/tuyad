TARGET:=tuyad
CFILES:=$(wildcard *.c)

.PHONY: all clean

all: $(TARGET)

#CFLAGS += -Wl,-E -I$(HOME)/tuya-iot-core-sdk/include -I$(HOME)/tuya-iot-core-sdk/interface -I$(HOME)/tuya-iot-core-sdk/utils
#LD_FLAGS += -L$(HOME)/tuya-iot-core-sdk/build/lib -Wl,-rpath=$(HOME)/tuya-iot-core-sdk/build/lib -llink_core -lutils_modules -lubox -lubus -lmiddleware_implementation -lcjson -llua -lm -ldl
LD_FLAGS += -llink_core -lutils_modules -lubox -lubus -largp-standalone -llua

$(TARGET): $(CFILES)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TARGET) $(CFILES) $(LD_FLAGS)

clean:
	rm -f $(TARGET)
	rm -f FileWithCloudInput.txt