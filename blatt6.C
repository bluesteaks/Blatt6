// PUSH-RELABEL-Algorithmus (Goldberg-Tarjan)
#include "wgraph.h"

using namespace std;

int main(int argc, char* argv[])
{
    	if (argc > 1) 
    	{
            Weighted_Graph g(argv[1],1);

            //flow Fluss, dm Distanzmarker, excess Überschuss, active speichert aktive Knoten
            int flow[g.num_edges()], dm[g.num_nodes()], excess[g.num_nodes()], active[g.num_nodes()], maxlevel;

            //level speichert pro dm-Wert alle Knoten auf diesem level
            vector<Graph::NodeId> level[2*g.num_nodes()];

            //Knoten initialisieren
            for (int i=0;i<g.num_nodes();i++)
            {
                active[i]=0;
            	dm[i]=0;
                excess[i]=0;
            }
            dm[0]=g.num_nodes();

            //Für jede Kante Fluss etc. initialisieren
            for (int e=0;e<g.num_edges();e++)
            {
            	Graph::EdgeId v=g.get_edge(e).get_tail();
                Graph::EdgeId w=g.get_edge(e).get_head();

                if(v==0)
                {
                	//von v ausgehende Kanten saturieren
					flow[e]=g._weight[e];
					excess[v]-=g._weight[e];
					excess[w]+=g._weight[e];

                    //neue aktive Knoten finden
                    if(w>1)
                    {
                        active[w]=1;
                        level[0].push_back(w);
                    }
                }
                else
                    flow[e]=0;
            }

            maxlevel=0;

            //so lange wir auf einem level noch mind. einen Knoten haben
            while(maxlevel!=0 || !level[0].empty())
            {
            	Graph::NodeId v=level[maxlevel].back();
            	level[maxlevel].pop_back();

				//falls Knoten aktiv (bei deaktivieren von einem Knoten müsste man ihn aus level entfernen
				//das würde aber viel Laufzeit kosten, da man jedes Level nach inaktiven durchsuchen muss)
            	if(active[v])
        		{
        			//versuche erlaubte Kante zu finden
        			Graph::EdgeId e=-1;
        			for(int i=0; i<g.get_node(v).out_edges().size() && e==-1;i++)
        			{
        				Graph::EdgeId t=g.get_node(v).out_edges()[i];
        				if(dm[v]==dm[g.get_edge(t).get_head()]+1 && flow[t]<g._weight[t])
        					e=t;
        			}

        			for(int i=0; i<g.get_node(v).in_edges().size() && e==-1;i++)
        			{
        				Graph::EdgeId t=g.get_node(v).in_edges()[i];
        				if(dm[v]==dm[g.get_edge(t).get_tail()]+1 && flow[t]>0)
        					e=t;
        			}

        			//PUSH (erlaubte Kante gefunden)
        			if(e!=-1)
        			{
	        			Graph::NodeId u=g.get_edge(e).get_tail();
						Graph::NodeId w=g.get_edge(e).get_head();

	        			int change;
	        			//e Vorwärtskante
	        			if(g.get_edge(e).get_tail()==v)
	        				change=min(int(g._weight[e])-flow[e],excess[v]);
	        			//e Rückkante
	        			else
	        				change=-min(flow[e],excess[v]);

						flow[e]+=change;
						excess[u]-=change;
						excess[w]+=change;

						//eventuell neuen aktiven Knoten oder alten Knoten deaktivieren
						if(active[u] && excess[u]<=0)
							active[u]=0;

						if(active[w] && excess[w]<=0)
							active[w]=0;

						if(excess[u]>0 && u>1)
						{
							level[dm[u]].push_back(u);
							maxlevel=max(maxlevel,dm[u]);
							active[u]=1;
						}

						if(excess[w]>0 && w>1)
						{
							level[dm[w]].push_back(w);
							maxlevel=max(maxlevel,dm[w]);
							active[w]=1;
						}
	        		}
	        		//RELABEL
	        		else
	        		{
	        			//finde minimales dm[w]+1
	        			int mindm=200000;
						for(int i=0; i!=g.get_node(v).out_edges().size();i++)
						{
							Graph::NodeId e=g.get_node(v).out_edges()[i];
							Graph::NodeId w=g.get_edge(e).get_head();

							if(flow[e]<g._weight[e])
								mindm=min(mindm,dm[w]+1);
						}

						for(int i=0; i!=g.get_node(v).in_edges().size();i++)
						{
							Graph::NodeId e=g.get_node(v).in_edges()[i];
							Graph::NodeId w=g.get_edge(e).get_tail();

							if(flow[e]>0)
								mindm=min(mindm,dm[w]+1);
						}

						dm[v]=mindm;
						level[dm[v]].push_back(v);
						active[v]=1;

						maxlevel=max(maxlevel,mindm);
	        		}
        		}

        		//falls maxlevel leer, gehe solange runter bis level mit aktiven Knoten vorhanden
                while(level[maxlevel].empty() && maxlevel>0)
            	    maxlevel--;            
            }

		    //Output
		    stringstream output;
		    int sum=0;
		    for(int i=0;i<g.num_edges();i++)
		            if(flow[i]>0)
		            {
		                  if(g.get_edge(i).get_head()==1)
		                        sum+=flow[i];
		                  //output << i << " " << flow[i] << "\n";
		            }
		    cout << sum <<"\n"<<output.str();
	    }
    return 0;
}
