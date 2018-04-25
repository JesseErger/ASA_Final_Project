################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/camera.c \
../src/color.c \
../src/converter.c \
../src/errorhandler.c \
../src/filehandler.c \
../src/light.c \
../src/lightRay.c \
../src/list.c \
../src/lodepng.c \
../src/main.c \
../src/material.c \
../src/obj.c \
../src/obj_parser.c \
../src/platform.c \
../src/raytrace.c \
../src/renderer.c \
../src/scene.c \
../src/sphere.c \
../src/string_extra.c \
../src/transforms.c \
../src/vector.c 

OBJS += \
./src/camera.o \
./src/color.o \
./src/converter.o \
./src/errorhandler.o \
./src/filehandler.o \
./src/light.o \
./src/lightRay.o \
./src/list.o \
./src/lodepng.o \
./src/main.o \
./src/material.o \
./src/obj.o \
./src/obj_parser.o \
./src/platform.o \
./src/raytrace.o \
./src/renderer.o \
./src/scene.o \
./src/sphere.o \
./src/string_extra.o \
./src/transforms.o \
./src/vector.o 

C_DEPS += \
./src/camera.d \
./src/color.d \
./src/converter.d \
./src/errorhandler.d \
./src/filehandler.d \
./src/light.d \
./src/lightRay.d \
./src/list.d \
./src/lodepng.d \
./src/main.d \
./src/material.d \
./src/obj.d \
./src/obj_parser.d \
./src/platform.d \
./src/raytrace.d \
./src/renderer.d \
./src/scene.d \
./src/sphere.d \
./src/string_extra.d \
./src/transforms.d \
./src/vector.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


