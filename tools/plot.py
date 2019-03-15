import sys
import graphviz

graph = graphviz.Digraph()

vec = [int(_) for _ in sys.argv[2:]]

N = (len(vec)-1)//2

def label(x):
    if x<N:
        return 'Unit %s'%(x+1)
    elif x==N:
        return 'Tailings'
    else:
        return 'Concentrate'

graph.attr(rankdir='LR')
graph.attr('node', shape='rectangle')

graph.edge('Feed', label(vec[0]), color='blue',
           headport='w', tailport='e',
           arrowhead='normal', arrowtail='normal')

for _ in range(N):
    graph.edge(label(_), label(vec[2*_+2]), color='blue',
               headport='w', tailport='n',
               arrowhead='normal')
    graph.edge(label(_), label(vec[2*_+1]), color='red',
               headport='w', tailport='s',
               arrowhead='normal')

with graph.subgraph() as s:
    s.attr(rank = 'source')
    for n in ('Feed',):
        s.node(n)

with graph.subgraph() as s:
    s.attr(rank = 'sink')
    for n in ('Tailings', 'Concentrate'):
        s.node(n)

graph.attr(label="vector: %s"%vec)
graph.render(filename=sys.argv[1], cleanup=True)
