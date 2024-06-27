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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/inverter/Logger.o \
	${OBJECTDIR}/inverter/Sofar.o \
	${OBJECTDIR}/lib/ConfigFile.o \
	${OBJECTDIR}/lib/ModbusAdu.o \
	${OBJECTDIR}/src/BrokerService.o \
	${OBJECTDIR}/src/Config.o \
	${OBJECTDIR}/src/MqttService.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/threadInverter.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/lib/tests/mqtttest.o

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
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/inverterbroker ${OBJECTFILES} ${LDLIBSOPTIONS} -lmqtt

${OBJECTDIR}/inverter/Logger.o: inverter/Logger.c
	${MKDIR} -p ${OBJECTDIR}/inverter
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inverter/Logger.o inverter/Logger.c

${OBJECTDIR}/inverter/Sofar.o: inverter/Sofar.c
	${MKDIR} -p ${OBJECTDIR}/inverter
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inverter/Sofar.o inverter/Sofar.c

${OBJECTDIR}/lib/ConfigFile.o: lib/ConfigFile.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ConfigFile.o lib/ConfigFile.c

${OBJECTDIR}/lib/ModbusAdu.o: lib/ModbusAdu.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ModbusAdu.o lib/ModbusAdu.c

${OBJECTDIR}/src/BrokerService.o: src/BrokerService.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BrokerService.o src/BrokerService.c

${OBJECTDIR}/src/Config.o: src/Config.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Config.o src/Config.c

${OBJECTDIR}/src/MqttService.o: src/MqttService.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MqttService.o src/MqttService.c

${OBJECTDIR}/src/main.o: src/main.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/threadInverter.o: src/threadInverter.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threadInverter.o src/threadInverter.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/lib/tests/mqtttest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   


${TESTDIR}/lib/tests/mqtttest.o: lib/tests/mqtttest.c 
	${MKDIR} -p ${TESTDIR}/lib/tests
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/lib/tests/mqtttest.o lib/tests/mqtttest.c


${OBJECTDIR}/inverter/Logger_nomain.o: ${OBJECTDIR}/inverter/Logger.o inverter/Logger.c 
	${MKDIR} -p ${OBJECTDIR}/inverter
	@NMOUTPUT=`${NM} ${OBJECTDIR}/inverter/Logger.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inverter/Logger_nomain.o inverter/Logger.c;\
	else  \
	    ${CP} ${OBJECTDIR}/inverter/Logger.o ${OBJECTDIR}/inverter/Logger_nomain.o;\
	fi

${OBJECTDIR}/inverter/Sofar_nomain.o: ${OBJECTDIR}/inverter/Sofar.o inverter/Sofar.c 
	${MKDIR} -p ${OBJECTDIR}/inverter
	@NMOUTPUT=`${NM} ${OBJECTDIR}/inverter/Sofar.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inverter/Sofar_nomain.o inverter/Sofar.c;\
	else  \
	    ${CP} ${OBJECTDIR}/inverter/Sofar.o ${OBJECTDIR}/inverter/Sofar_nomain.o;\
	fi

${OBJECTDIR}/lib/ConfigFile_nomain.o: ${OBJECTDIR}/lib/ConfigFile.o lib/ConfigFile.c 
	${MKDIR} -p ${OBJECTDIR}/lib
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/ConfigFile.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ConfigFile_nomain.o lib/ConfigFile.c;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/ConfigFile.o ${OBJECTDIR}/lib/ConfigFile_nomain.o;\
	fi

${OBJECTDIR}/lib/ModbusAdu_nomain.o: ${OBJECTDIR}/lib/ModbusAdu.o lib/ModbusAdu.c 
	${MKDIR} -p ${OBJECTDIR}/lib
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/ModbusAdu.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ModbusAdu_nomain.o lib/ModbusAdu.c;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/ModbusAdu.o ${OBJECTDIR}/lib/ModbusAdu_nomain.o;\
	fi

${OBJECTDIR}/src/BrokerService_nomain.o: ${OBJECTDIR}/src/BrokerService.o src/BrokerService.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/BrokerService.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BrokerService_nomain.o src/BrokerService.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/BrokerService.o ${OBJECTDIR}/src/BrokerService_nomain.o;\
	fi

${OBJECTDIR}/src/Config_nomain.o: ${OBJECTDIR}/src/Config.o src/Config.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Config.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Config_nomain.o src/Config.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Config.o ${OBJECTDIR}/src/Config_nomain.o;\
	fi

${OBJECTDIR}/src/MqttService_nomain.o: ${OBJECTDIR}/src/MqttService.o src/MqttService.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/MqttService.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MqttService_nomain.o src/MqttService.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/MqttService.o ${OBJECTDIR}/src/MqttService_nomain.o;\
	fi

${OBJECTDIR}/src/main_nomain.o: ${OBJECTDIR}/src/main.o src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main_nomain.o src/main.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/main.o ${OBJECTDIR}/src/main_nomain.o;\
	fi

${OBJECTDIR}/src/threadInverter_nomain.o: ${OBJECTDIR}/src/threadInverter.o src/threadInverter.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/threadInverter.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O2 -I./include -I./src -I./lib -I./inverter -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threadInverter_nomain.o src/threadInverter.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/threadInverter.o ${OBJECTDIR}/src/threadInverter_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
