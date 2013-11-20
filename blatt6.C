// PUSH-RELABEL-Algorithmus (Goldberg-Tarjan)
#include "wgraph.h"
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
      if (argc > 1) 
      {
            Weighted_Graph g(argv[1],1);

            //flow Fluss, dm Distanzmarker, excess Überschuss, backwards zeigt an ob Kante oder Rückkante
            int flow[g.num_edges()], dm[g.num_nodes()], excess[g.num_nodes()],backwards[g.num_edges()], maxlevel;
            //residualback Residualkap. der Rückkante, active zeigt an ob Knoten aktiv
            int residual[g.num_edges()],residualback[g.num_edges()], active[g.num_nodes()];

            //level speichert pro dm-wert die aktiven Knoten
            vector<Graph::NodeId> level[2*g.num_nodes()];
            vector<Graph::EdgeId> legaledges[g.num_nodes()];


            //Distanzmarker und Fluss initialisieren
            dm[0]=g.num_nodes();
            excess[0]=0;
            active[0]=0;

            maxlevel=0;

            for (int i=1;i<g.num_nodes();i++)
            {
                  active[i]=0;
            	dm[i]=0;
                  excess[i]=0;
            }

            //Für jede Kante Fluss, Residualkap. etc. initialisieren
            for (int i=0;i<g.num_edges();i++)
            {
                  residual[i]=g._weight[i];
                  residualback[i]=0;
                  backwards[i]=0;

                  Graph::EdgeId v=g.get_edge(i).get_tail();
                  Graph::EdgeId w=g.get_edge(i).get_head();

                  //von v ausgehende Kanten saturieren
                  if(v==0)
                  {
                        flow[i]=g._weight[i];
                        residual[i]-=g._weight[i];
                        residualback[i]+=g._weight[i];
                        excess[w]+=g._weight[i];

                        //neue aktive Knoten finden
                        if(w>1 && excess[w]>0 && !active[w])
                        {
                              active[w]=1;
                              level[dm[w]].push_back(w);
                        }
                  }
                  else
                        flow[i]=0;
            }

            //eigentlicher Alg.
            while(maxlevel!=0 || !level[0].empty())
            {
                  Graph::NodeId v=level[maxlevel].back();
                  level[maxlevel].pop_back();
                  active[v]=0;
                              
                  //PUSH
                  if(!legaledges[v].empty())
                  {
                        Graph::EdgeId e=legaledges[v].back();
                        legaledges[v].pop_back();

                        Graph::NodeId u=g.get_edge(e).get_tail();
                        Graph::NodeId w=g.get_edge(e).get_head();

                        //Falls e keine Rückkante im Residualgraph
                        if(!backwards[e])
                        {
                              int change=min(residual[e],excess[v]);
      
                              flow[e]+=change;

                              residual[e]-=change;
                              residualback[e]+=change;

                              excess[v]-=change;
                              excess[w]+=change;

                              //e noch im Residualgraph vorhanden
                              if(residual[e]>0)
                                    legaledges[v].push_back(e);
                        }
                        //e ist Rückkante im Residualgraph
                        else
                        {
                              backwards[e]=0;
                              int change=min(residualback[e],excess[v]);

                              flow[e]-=change;

                              residual[e]+=change;
                              residualback[e]-=change;

                              excess[v]-=change;
                              excess[u]+=change;

                              //e noch vorhanden
                              if(residualback[e]>0)
                              {
                                    backwards[e]=1;
                                    legaledges[v].push_back(e);
                              }
                        }

                        //prüfe ob neue aktive Knoten entstanden sind
                        if(!active[u] && excess[u]>0 && u>1)
                        {
                              maxlevel=min(dm[u],maxlevel);
                              level[dm[u]].push_back(u);
                              active[u]=1;
                        }
                        if(!active[w] && excess[w]>0 && w>1)
                        {
                              maxlevel=min(dm[v],maxlevel);
                              level[dm[w]].push_back(w);
                              active[w]=1;
                        }
                  }
                  //RELABEL
                  else
                  {
                        int mindm=200000;

                        //finde minimales dm[w] für alle Kanten (v,w) im Residualgraph
                        for(int i=0;i!=g.get_node(v).out_edges().size();i++)
                        {
                              Graph::EdgeId e=g.get_node(v).out_edges()[i];
                              Graph::NodeId w=g.get_edge(e).get_head();

                              if(residual[e]>0)
                                    mindm=min(mindm,dm[w]+1);
                        }

                        for(int i=0;i!=g.get_node(v).in_edges().size();i++)
                        {
                              Graph::EdgeId e=g.get_node(v).in_edges()[i];
                              Graph::NodeId w=g.get_edge(e).get_tail();

                              if(residualback[e]>0) 
                                    mindm=min(mindm,dm[w]+1);
                        }

                        //v kommt auf neues dm-level (eventuell maximal)
                        dm[v]=mindm;
                        maxlevel=max(maxlevel,dm[v]);
                        level[dm[v]].push_back(v);
                        active[v]=1;

                        //legal edges aktualisieren
                        //alte Kanten könnten nicht mehr erlaubt sein
                        legaledges[v].clear();

                        for(int i=0;i!=g.get_node(v).out_edges().size();i++)
                        {
                              Graph::EdgeId e=g.get_node(v).out_edges()[i];
                              Graph::NodeId w=g.get_edge(e).get_head();

                              //e noch im Residualgraph?
                              if((dm[v]==dm[w]+1) && residual[e]>0)
                              {
                                    legaledges[v].push_back(e);
                                    backwards[e]=0; //e keine Rückkante
                              }
                        }

                        for(int i=0;i!=g.get_node(v).in_edges().size();i++)
                        {
                              Graph::EdgeId e=g.get_node(v).in_edges()[i];
                              Graph::NodeId w=g.get_edge(e).get_tail();

                              //e noch im Residualgraph?
                              if((dm[v]==dm[w]+1) && residualback[e]>0)
                              {
                                    legaledges[v].push_back(e);
                                    backwards[e]=1; //e ist Rückkante
                              }
                        }
                  }

                  while(level[maxlevel].empty() && maxlevel>0)
                        maxlevel--;            
            }

      //Output
      stringstream output;
      int sum=0;
      for(int i=0;i<g.num_edges();i++)
            if(flow[i]>0)
            {
                  if(g.get_edge(i).get_tail()==0)
                        sum+=flow[i];
                  output << i << " " << flow[i] << "\n";
            }
      cout << sum << " " << g.num_edges() << "\n" ;
      }
      return 0;
}
