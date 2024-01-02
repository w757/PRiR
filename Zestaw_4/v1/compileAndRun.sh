#!/bin/bash

c++ -O4 -fopenmp DataSupplier.cpp Force.cpp main.cpp MyForce.cpp SimpleDataSupplier.cpp Simulation.cpp && ./a.out
