// PUSH-RELABEL-Algorithmus (Goldberg-Tarjan)
#include "wgraph.h"

using namespace std;

vector<Graph::EdgeId> updatelegaledges(Weighted_Graph g, Graph::NodeId v,int flow[],int dm[])
{
      vector<Graph::EdgeId> legaledges;
      for(int i=0;i!=g.get_node(v).out_edges().size();i++)
      {
            Graph::EdgeId e=g.get_node(v).out_edges()[i];
            Graph::NodeId w=g.get_edge(e).get_head();

            //e noch im Residualgraph?
            if((dm[v]==dm[w]+1) && (g._weight[e]-flow[e]>0))
                 legaledges.push_back(e);
      }

      for(int i=0;i!=g.get_node(v).in_edges().size();i++)
      {
            Graph::EdgeId e=g.get_node(v).in_edges()[i];
            Graph::NodeId w=g.get_edge(e).get_tail();

            //e noch im Residualgraph?
            if((dm[v]==dm[w]+1) && flow[e]>0)
                  legaledges.push_back(e);
      }

      return legaledges;
}

int main(int argc, char* argv[])
{
      if (argc > 1) 
      {
            Weighted_Graph g(argv[1],1);

            //flow Fluss, dm Distanzmarker, excess Überschuss, backwards zeigt an ob Kante oder Rückkante
            int flow[g.num_edges()], dm[g.num_nodes()], excess[g.num_nodes()],/*backwards[g.num_edges()]*/ maxlevel;
            //residualback Residualkap. der Rückkante, active zeigt an ob Knoten aktiv
            int active[g.num_nodes()];

            //level speichert pro dm-wert die aktiven Knoten
            vector<int> backwards[g.num_edges()];
            vector<Graph::NodeId> level[2*g.num_nodes()];
            vector<Graph::EdgeId> legaledges[g.num_nodes()];


            //Distanzmarker und Fluss initialisieren
            maxlevel=0;

            for (int i=0;i<g.num_nodes();i++)
            {
                  active[i]=0;
            	dm[i]=0;
                  excess[i]=0;
                  for(int j=0;j<g.num_edges();j++)
                        backwards[i].push_back(0);
            }

            dm[0]=g.num_nodes();

            //Für jede Kante Fluss, Residualkap. etc. initialisieren
            for (int i=0;i<g.num_edges();i++)
            {

                  Graph::EdgeId v=g.get_edge(i).get_tail();
                  Graph::EdgeId w=g.get_edge(i).get_head();

                  backwards[v][i]=0;
                  backwards[w][i]=1;

                  //von v ausgehende Kanten saturieren
                  if(v==0)
                  {
                        flow[i]=g._weight[i];
                        excess[w]+=g._weight[i];
                        excess[0]-=g._weight[i];

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
                /*  for(int j=0;j<=maxlevel;j++)
                  {
                        cout << "level "<<j<< " : ";
                  for (int i=0;i!=level[j].size();i++)
                        cout << level[j][i] << " ";
                  cout << "\n";
                  }*/
                  Graph::NodeId v=level[maxlevel].back();
                  level[maxlevel].pop_back();
               //   cout << v << " gewählt, level "<< dm[v] << " edges " << legaledges[v].size() << "\n";

                  if(active[v])
                  {
                        //PUSH
                        if(!legaledges[v].empty())
                        {
                              Graph::EdgeId e=legaledges[v].back();
                              legaledges[v].pop_back();

                              Graph::NodeId w;

                              //Falls e keine Rückkante im Residualgraph
                              int change,res;
                              if(!backwards[v][e])
                              {
                                    change=min(int(g._weight[e]-flow[e]),excess[v]);
                                    res=change;
                                    w=g.get_edge(e).get_head();
                              }
                              //e ist Rückkante im Residualgraph
                              else
                              {
                                    change=-min(flow[e],excess[v]);
                                    res=-change;
                                    w=g.get_edge(e).get_tail();
                              }

                              flow[e]+=change;

                              excess[v]-=res;
                              excess[w]+=res;

                              //e noch im Residualgraph vorhanden
                              if(g._weight[e]-flow[e]>0 && flow[e]!=0)
                                    legaledges[v].push_back(e);

                              if(excess[w]>0 && w>1 && !active[w])
                              {
                                    active[w]=1;
                                    level[dm[w]].push_back(w);
                                    //legaledges[w]=updatelegaledges(g,w,flow,dm);
                              }

                              if(excess[v]>0)
                                          level[dm[v]].push_back(v);
                                    else
                                          active[v]=0;
                              
                              if(excess[w]<=0 && w>1)
                              {     
                                    active[w]=0;
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

                                    if(g._weight[e]-flow[e]>0)
                                          mindm=min(mindm,dm[w]+1);
                              }

                              for(int i=0;i!=g.get_node(v).in_edges().size();i++)
                              {
                                    Graph::EdgeId e=g.get_node(v).in_edges()[i];
                                    Graph::NodeId w=g.get_edge(e).get_tail();

                                    if(flow[e]>0) 
                                          mindm=min(mindm,dm[w]+1);
                              }

                              //v kommt auf neues dm-level (eventuell maximal)
                              dm[v]=mindm;
                              maxlevel=max(maxlevel,dm[v]);
                              level[dm[v]].push_back(v);
                              active[v]=1;

                              //legal edges aktualisieren
                              //alte Kanten könnten nicht mehr erlaubt sein
                              legaledges[v]=updatelegaledges(g,v,flow,dm);
                              /*for(int i=0;i!=g.get_node(v).out_edges().size();i++)
                                    legaledges[g.get_edge(g.get_node(v).out_edges()[i]).get_head()]=updatelegaledges(g,g.get_edge(g.get_node(v).out_edges()[i]).get_head(),flow,dm);

                              for(int i=0;i!=g.get_node(v).in_edges().size();i++)
                                    legaledges[g.get_edge(g.get_node(v).in_edges()[i]).get_tail()]=updatelegaledges(g,g.get_edge(g.get_node(v).in_edges()[i]).get_tail(),flow,dm);
                        */}
                  }
                  while(level[maxlevel].empty() && maxlevel>0)
                        maxlevel--;            
            }

      //Output
      stringstream output;
      int sum=0,sum2=0;
      for(int i=0;i<g.num_edges();i++)
            if(flow[i]>0)
            {
                  if(g.get_edge(i).get_head()==1)
                        sum+=flow[i];
                  if(g.get_edge(i).get_tail()==0)
                        sum2+=flow[i];
                  output << i << " " << flow[i] << "\n";
            }
      cout << sum << " " << sum2 << "\n" ;
      }
      return 0;
}
