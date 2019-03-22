CPPFLAGS = "-I${GRAPHVIZ_HOME}/include"
LDFLAGS = "-L${GRAPHVIZ_HOME}/lib"

plot: plot.cpp
	${CXX} ${CPPFLAGS} ${CXXFLAGS} plot.cpp ${LDFLAGS} -lgvc -lcgraph -o plot
