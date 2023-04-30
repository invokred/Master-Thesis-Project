##
#	Sample Makefile from the fesif project, needs adjustment to the new directory structure,
#	Author: Archisman Pathak & Aniket Kumar(Slightly)
##
CC = gcc
# CXX = /usr/bin/g++
CXX = /usr/bin/g++-11
OR_LIBS = -L./lib -lortools

CFLAGS = --std=c++17 -W -Wall -Wno-sign-compare -O4 -pipe -mmmx -msse -msse2 -msse3 -g -Iinclude/ortools -Iinclude -I. # -mcmodel=medium
# CFLAGS = 
OR_CFLAGS = -v -fPIC -std=c++17  -DNDEBUG -Iinclude/ortools -Iinclude -I. -g -DARCH_K8 -Wno-deprecated -DUSE_BOP -DUSE_GLOP -DUSE_CBC -DUSE_CLP -DUSE_SCIP
OR_TOOLS_LNK = -Llib -lortools

LDFLAGS = -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../lib -lz -lglog
MEM = -D WATCH_MEM

CLUS_INC_DIR = include/clustering
FESIF_INCLUDE_DIR = $(CLUS_INC_DIR)/fesif
CLUS_SRC_DIR = src/clustering
FESIF_SRC_DIR = $(CLUS_SRC_DIR)/fesif

CLARKE_INCLUDE_DIR = include/clustering/Clarke
CLARKE_SRC_DIR = src/clustering/Clarke
TSP_SRC_DIR = src/routeplan
TSP_INCLUDE_DIR = include/routeplan
OPT_INCLUDE_DIR = src
OPT_HEADER_DIR = include
HGS_INCLUDE_DIR = $(CLUS_INC_DIR)/HGS
HGS_SRC_DIR = $(CLUS_SRC_DIR)/HGS

RP_INC_DIR = include/routeplan
# TSP_OR_INCLUDE_DIR = $(RP_INC_DIR)/
RP_SRC_DIR = src/routeplan


all: Integrate 
#  Build FESIF Code 
global.o: $(FESIF_SRC_DIR)/global.cpp
	$(CXX) $(CFLAGS) -c $(FESIF_SRC_DIR)/global.cpp $(LIBS)
	
HST.o: $(FESIF_SRC_DIR)/HST.cpp $(FESIF_SRC_DIR)/global.cpp global.o
	$(CXX) $(CFLAGS) -c $(FESIF_SRC_DIR)/HST.cpp $(LIBS)
	
utils.o: $(FESIF_INCLUDE_DIR)/utils.h $(FESIF_SRC_DIR)/utils.cpp $(FESIF_INCLUDE_DIR)/HST.h $(FESIF_SRC_DIR)/HST.cpp HST.o $(FESIF_INCLUDE_DIR)/global.h $(FESIF_SRC_DIR)/global.cpp global.o
	$(CXX) $(CFLAGS) -c $(FESIF_SRC_DIR)/utils.cpp $(LIBS)
	
chst: $(FESIF_SRC_DIR)/constructHSTs.cpp $(FESIF_INCLUDE_DIR)/global.h $(FESIF_SRC_DIR)/global.cpp global.o $(FESIF_INCLUDE_DIR)/HST.h $(FESIF_SRC_DIR)/HST.cpp HST.o
	$(CXX) $(CFLAGS) -o chst global.o HST.o $(FESIF_SRC_DIR)/constructHSTs.cpp $(LIBS) $(MEM)

fesif.o:  $(FESIF_SRC_DIR)/FESIF.cpp  $(FESIF_INCLUDE_DIR)/utils.h $(FESIF_SRC_DIR)/utils.cpp utils.o $(FESIF_INCLUDE_DIR)/HST.h $(FESIF_SRC_DIR)/HST.cpp HST.o $(FESIF_INCLUDE_DIR)/global.h $(FESIF_SRC_DIR)/global.cpp global.o
	$(CXX) $(CFLAGS) -c $(FESIF_SRC_DIR)/FESIF.cpp $(LIBS) 

TSP_LK.o:
	$(CXX) $(CFLAGS) -c src/routeplan/TSP_LK.cpp $(LIBS)

# Build TSP Code
TSP_OR.o: 
# $(CXX) -v -c $(RP_SRC_DIR)/TSP_OR.cpp $(CFLAGS) $(OR_CFLAGS) $(LDFLAGS) $(OR_LIBS) $(OR_TOOLS_LNK)  
	/usr/bin/g++-11 -v -c -Iinclude/ortools -Iinclude -I. src/routeplan/TSP_OR.cpp --std=c++17 -W -Wall -Wno-sign-compare -O4 -pipe -mmmx -msse -msse2 -msse3 -g -DNDEBUG -DARCH_K8 -Wno-deprecated -DUSE_BOP -DUSE_GLOP -DUSE_CBC -DUSE_CLP -DUSE_SCIP -lz -lglog -L./lib -Llib -lortools

TSP_OR_EDD.o: 
# $(CXX) -v -c $(RP_SRC_DIR)/TSP_OR.cpp $(CFLAGS) $(OR_CFLAGS) $(LDFLAGS) $(OR_LIBS) $(OR_TOOLS_LNK)  
	/usr/bin/g++-11 -v -c -Iinclude/ortools -Iinclude -I. src/routeplan/TSP_OR_EDD.cpp --std=c++17 -W -Wall -Wno-sign-compare -O4 -pipe -mmmx -msse -msse2 -msse3 -g -DNDEBUG -DARCH_K8 -Wno-deprecated -DUSE_BOP -DUSE_GLOP -DUSE_CBC -DUSE_CLP -DUSE_SCIP -lz -lglog -L./lib -Llib -lortools
# Build the EB-AFIT algorithm
EB-AFIT.o:
	$(CXX) $(CFLAGS) -c src/binpack/EB-AFIT.cpp $(LIBS)

Optimiser.o:
	$(CXX) $(CFLAGS) -c src/Optimiser.cpp $(LIBS) 
DistMatrix.o:
	$(CXX) $(CFLAGS) -c src/DistMatrix.cpp -lcurl $(LIBS) 

AlgorithmParameters.o: $(HGS_SRC_DIR)/AlgorithmParameters.cpp
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/AlgorithmParameters.cpp 
C_Interface.o :  $(HGS_SRC_DIR)/C_Interface.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/C_Interface.cpp 
Params.o : $(HGS_SRC_DIR)/Params.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/Params.cpp 
Individual.o : $(HGS_SRC_DIR)/Params.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/Individual.cpp 
LocalSearch.o : $(HGS_SRC_DIR)/LocalSearch.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/LocalSearch.cpp 
Population.o : $(HGS_SRC_DIR)/Population.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/Population.cpp 
Split.o : $(HGS_SRC_DIR)/Split.cpp 
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/Split.cpp 
Genetic.o : $(HGS_SRC_DIR)/Genetic.cpp
	$(CXX)  -fPIC -c $(HGS_SRC_DIR)/Genetic.cpp
InstanceCVRPLIB.o : $(HGS_SRC_DIR)/InstanceCVRPLIB.cpp 
	$(CXX) -fPIC $(CFLAGS) -c $(HGS_SRC_DIR)/InstanceCVRPLIB.cpp $(LIBS)
HGS.o: $(HGS_SRC_DIR)/HGS.cpp
	$(CXX) -fPIC $(CFLAGS) -c $(HGS_SRC_DIR)/HGS.cpp
# SelfClustering Algorithm
cluster.o : 
	$(CXX) $(CFLAGS) -c src/clustering/selfClustering/cluster.cpp $(LIBS)

TSP_CK.o :
	$(CXX) $(CFLAGS) -c src/routeplan/TSP_CK.cpp $(LIBS)

clarke.o : 
	$(CXX) $(CFLAGS) -c src/clustering/Clarke/clarke.cpp $(LIBS)

libHGS.so : AlgorithmParameters.o C_Interface.o Params.o Individual.o LocalSearch.o Population.o Split.o Genetic.o InstanceCVRPLIB.o HGS.o
	$(CXX)  -shared -fPIC -o ./lib/libHGS.so AlgorithmParameters.o C_Interface.o Params.o Individual.o LocalSearch.o Population.o Split.o Genetic.o InstanceCVRPLIB.o HGS.o
# Build the executable
Integrate: main.cpp fesif.o TSP_OR.o EB-AFIT.o TSP_OR_EDD.o Optimiser.o DistMatrix.o $(CLARKE_INCLUDE_DIR)/clarke.hpp $(CLARKE_SRC_DIR)/clarke.cpp $(TSP_SRC_DIR)/TSP_CK.cpp $(TSP_INCLUDE_DIR)/TSP_CK.hpp $(TSP_SRC_DIR)/TSP_LK.cpp $(RP_INC_DIR)/TSP_LK.hpp libHGS.so
	/usr/bin/g++-11 --std=c++17 -W -Wall -Wno-sign-compare -O4 -pipe -mmmx -msse -msse2 -msse3 -g -Iinclude/ortools -Iinclude -I. global.o HST.o utils.o FESIF.o TSP_OR.o TSP_OR_EDD.o EB-AFIT.o Optimiser.o DistMatrix.o main.cpp $(CLARKE_SRC_DIR)/clarke.cpp $(TSP_SRC_DIR)/TSP_CK.cpp $(TSP_SRC_DIR)/TSP_LK.cpp -o Integrate -Llib  -lortools -lHGS -lcurl


.PHONY: clean
clean:
		-@rm *.o *.gcno *.so *~ 2> /dev/null || true
		rm ./lib/libHGS.so
		-@rm fesif chst 2> /dev/null || true
		rm Integrate
.PHONY: distclean
distclean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm fesif chst 2> /dev/null || true
		
obj_remove:
	-@rm *.o *.gcno *.so *~ 2> /dev/null || true
# Integrate: main.cpp  $(OPT_INCLUDE_DIR)/Optimiser.cpp $(OPT_HEADER_DIR)/Optimiser.hpp
# 	$(CXX) $(CFLAGS) main.cpp  $(OPT_INCLUDE_DIR)/Optimiser.cpp -o Integrate



