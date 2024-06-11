#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/lib/ConfigFile.o \
	${OBJECTDIR}/lib/ModbusAdu.o \
	${OBJECTDIR}/src/ImeConfig.o \
	${OBJECTDIR}/src/ImeInverter.o \
	${OBJECTDIR}/src/ImeService.o \
	${OBJECTDIR}/src/LoggerSofar.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/inverterbroker

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/inverterbroker: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/inverterbroker ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/lib/ConfigFile.o: lib/ConfigFile.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ConfigFile.o lib/ConfigFile.c

${OBJECTDIR}/lib/ModbusAdu.o: lib/ModbusAdu.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ModbusAdu.o lib/ModbusAdu.c

${OBJECTDIR}/src/ImeConfig.o: src/ImeConfig.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ImeConfig.o src/ImeConfig.c

${OBJECTDIR}/src/ImeInverter.o: src/ImeInverter.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ImeInverter.o src/ImeInverter.c

${OBJECTDIR}/src/ImeService.o: src/ImeService.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ImeService.o src/ImeService.c

${OBJECTDIR}/src/LoggerSofar.o: src/LoggerSofar.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LoggerSofar.o src/LoggerSofar.c

${OBJECTDIR}/src/main.o: src/main.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DPARAM_SERVICE_DEBUG_INFO -I./include -I./src -I./lib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
