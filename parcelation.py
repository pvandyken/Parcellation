
# Copyright (C) 2019  Andrea Vázquez Varela

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

#Authors:
# Narciso López López
# Andrea Vázquez Varela
#Creation date: 31/01/2019
#Last update: 20/07/2019


import argparse
import IO
from utils import *
import networkx as nx
from classes import *

def label_triangles(triangles):

    for tri in triangles:
        label0 = tri.v1.label_parcel
        label1 = tri.v2.label_parcel
        label2 = tri.v3.label_parcel

        if (label0 == label1 == label2):
            tri.label_parcel = label2
        elif (label0 == label1 and label0!=label2):
            tri.label_parcel = label0
        elif (label1 == label2 and (label1!= label0)):
            tri.label_parcel = label1
        elif (label0 == label2) and (label0!=label1):
            tri.label_parcel = label0
        else:
            tri.label_parcel = label0

    return triangles


def fusion(aparcels,anatomic_parcel,sub_parcel_list,parcel_names):
    while(len(sub_parcel_list)!=1):
        sub_parcel1 = sub_parcel_list[0]
        sub_parcel2 = sub_parcel_list[1]
        new_name = fusion_names(parcel_names[sub_parcel1.label],parcel_names[sub_parcel2.label])
        new_triangles = union(sub_parcel1.triangles,sub_parcel2.triangles)
        new_fibers = union(sub_parcel1.fibers,sub_parcel2.fibers)
        new_connections = union(sub_parcel1.connected_parcels,sub_parcel2.connected_parcels) 
        sub_parcel1.triangles = new_triangles
        sub_parcel1.fibers = new_fibers
        sub_parcel1.connected_parcels = new_connections
        anatomic_parcel.remove_subparcel(sub_parcel2) 
        for sub_parcel in anatomic_parcel.sub_parcels: 
            for tri in sub_parcel.triangles:
                tri.replace_label(sub_parcel2.label,sub_parcel1.label)

        parcel_names[sub_parcel1.label] = new_name
        sub_parcel_list.remove(sub_parcel_list[1])  
    return parcel_names


def joinable_sparcels(clique, visited):
    subparcels = []
    for node in clique:
        if not visited[node.label]:
            subparcels.append(node)
            visited[node.label] = True
    return subparcels


def delete_reps(us,pct_matrix):
    cliques = []
    for clique in us:
        cliques.append(sorted(clique, key = lambda kv: kv.label))

    cliques = sorted(cliques, key = lambda kv: sum([sp.label for sp in kv]))

    rep_map = {}
    selected_cliques = {}
    for i,clique in enumerate(cliques):
        for sparcel in clique:
            if sparcel not in rep_map:
                rep_map[sparcel] = [i]
            else:
                rep_map[sparcel].append(i)
    for sparcel1,clique_list in rep_map.items():
        if len(clique_list) > 1:
            max_pct = 1
            selected_cliques[sparcel1] = -1
            for i in clique_list:
                clique = cliques[i]
                sum_pct = 0
                for sparcel2 in clique:
                    sum_pct+=pct_matrix[sparcel1.label][sparcel2.label]
                if sum_pct > max_pct:
                    max_pct = sum_pct
                    selected_cliques[sparcel1] = i

    for sparcel,index_clique in selected_cliques.items():
        for i in range(len(cliques)):
            if i !=index_clique and sparcel in cliques[i]:
                cliques[i].remove(sparcel)


    return (sorted(cliques, key = len, reverse = True))

def create_fusion_list(anatomic_parcel,thr_prob,thr_pct,names):
    fusion_list = []
    nsparcels = len(names)
    pct_matrix = np.zeros((nsparcels,nsparcels))
    connect_graph = nx.Graph()
    for sparcel in anatomic_parcel.sub_parcels:
        connect_graph.add_node(sparcel)
    for sparcel1 in anatomic_parcel.sub_parcels:
        for sparcel2  in anatomic_parcel.sub_parcels:
            if sparcel1!=sparcel2:
                triangles1 = sparcel1.get_triangles_prob(thr_prob,sparcel1.label)
                triangles2 = sparcel2.get_triangles_prob(thr_prob, sparcel2.label)
                inter = intersection(triangles1, triangles2)
                if (len(sparcel1.triangles)) > len(sparcel2.triangles):
                    pct = len(inter) / len(
                        sparcel2.triangles) 
                else:
                    pct = len(inter) / len(sparcel1.triangles) 
                if pct >= thr_pct:
                    connect_graph.add_edge(sparcel1,sparcel2)
                    pct_matrix[sparcel1.label][sparcel2.label] = pct
                    pct_matrix[sparcel2.label][sparcel1.label] = pct

    cliques = sorted(nx.find_cliques(connect_graph), key=len, reverse=True)
    cliques = delete_reps(cliques,pct_matrix)

    visited = np.zeros(len(names), np.bool)
    for clique in cliques:
        parcel_list = joinable_sparcels(clique,visited)
        fusion_list.append(parcel_list)

    return fusion_list


def recalc_probability(anatomic_parcel):
    for sub_parcel in anatomic_parcel.sub_parcels:
        for tri in sub_parcel.triangles:
            tri.set_prob_map(anatomic_parcel)


def remove_subparcel(aparcels,anatomic_parcel,subparcel):
    for sparcel in anatomic_parcel.sub_parcels:
        for tri in sparcel.triangles:
            tri.remove_labels([subparcel.label])
    anatomic_parcel.remove_subparcel(subparcel)


def remove_small_parcels(aparcels,anatomic_parcel,parcel_names,size_thr):
    avg_inter = 0
    avg_size = sum([len(sparcel.triangles) for sparcel in anatomic_parcel.sub_parcels]) / (len(anatomic_parcel.sub_parcels))
    for sparcel in anatomic_parcel.sub_parcels:
        avg_inter +=sum([len(tri.labels_subparcel) for tri in sparcel.triangles])
    avg_inter = avg_inter / (len(anatomic_parcel.sub_parcels))
    thr = (avg_size*size_thr) / 100
    thr_inter = (avg_inter*size_thr) / 100
    n = 0
    for sparcel in anatomic_parcel.sub_parcels:
        num_inters = sum([len(tri.labels_subparcel) for tri in sparcel.triangles])
        if (len(sparcel.triangles) < thr) or (num_inters < thr_inter):
            n+=1
            remove_subparcel(aparcels,anatomic_parcel,sparcel)
    return n


def processing_parcels(aparcels,pct,thr_prob,size_thr,parcel_names,trac,trac_path,hemi):
    n_remove = 0
    for i,anatomic_parcel in enumerate(aparcels):
        if len(anatomic_parcel.sub_parcels) > 0:
            if trac == "y":
                fusion_file = open(trac_path+"/"+hemi+"fusion.txt","a+")
            n_remove += remove_small_parcels(aparcels,anatomic_parcel,parcel_names,size_thr)
            recalc_probability(anatomic_parcel)
            fusion_list = create_fusion_list(anatomic_parcel,thr_prob,pct,parcel_names)

            for list in fusion_list:
                if (len(list)>1):
                    if trac == "y":
                        fusion_file.write("sp "+str(list[0].label)+"\nf ")
                        fusion_file.write(" ".join([str(parcel.label) for parcel in list])+"\n")
                    fusion(aparcels,anatomic_parcel,list,parcel_names)

def update_fibers_map(aparcels):
    for ap in aparcels:
        for sp in ap.sub_parcels:
            for fiber in sp.fibers:
                if fiber.tri_end in sp.triangles:
                    tri = fiber.tri_end
                elif fiber.tri_init in sp.triangles:
                    tri = fiber.tri_init
                if fiber.subject == "001":
                    tri.fibers_map[sp.label] = [fiber]


def get_hard_parcels(aparcels):
    map_hlabels =  {}
    map_slabels =  {}
    for ap in aparcels:
        hparcels_map = {}
        for sp in ap.sub_parcels:
            for tri in sp.triangles:
                selected_parcel = most_probable(tri.prob_map)
                if selected_parcel != -1:
                    hparcels_map[selected_parcel] = [tri]
        for label, triangles in hparcels_map.items():
            sp = ap.find_subparcel(label)
            map_hlabels[label] = sp
            hparcel = SubParcel(label, ap.label, triangles, [])
            map_slabels[label]=hparcel
            ap.hard_parcels.append(hparcel)
    for ap in aparcels:
        for hp in ap.hard_parcels:
            sp = map_hlabels[hp.label]
            for c in sp.connected_parcels:
                if c.label in map_slabels:
                    hp.connected_parcels.append(map_slabels[c.label])

def main():
    parser = argparse.ArgumentParser(description='Create parcel of multiple subjects')
    parser.add_argument('--Intersection-dir',type= str,help='Input intersection directory')
    parser.add_argument('--LVtk-file', type=str, help='Input file with the vtk')
    parser.add_argument('--RVtk-file', type=str, help='Input file with the vtk')
    parser.add_argument('--Lvlabels-file',type= str,help='Input file with the vertex labels')
    parser.add_argument('--Rvlabels-file',type= str,help='Input file with the vertex labels')
    parser.add_argument('--parcel-names',type= str, help='Input file with the names of the parcels')
    parser.add_argument('--output-dir', type=str, help='Output directory')
    parser.add_argument('--traceability',type= str, default='y', help='Write y, to obtain the traceability of the parcels')
    parser.add_argument('--size-thr', type=float, default='10',help='Size to delete small parcels')
    parser.add_argument('--density-center', type=float, default='0.15',help='Less probable triangles in a parcel (probability)')
    parser.add_argument('--pct', type=float, default='10',help='Percent of common triangles in the intersection of two density centers')
    args = parser.parse_args()
    atlas_path,trac_path,connect_path = IO.create_atlas_dirs(args.output_dir,args.traceability)

    trac = args.traceability.lower()

    Lparcel_lnames, Lparcel_names, Lanatomic_parcels = IO.read_parcel_names(args.parcel_names)
    Rparcel_lnames, Rparcel_names, Ranatomic_parcels = IO.read_parcel_names(args.parcel_names)

    Lvertex_labels = IO.read_vertex_labels(args.Lvlabels_file) 
    Rvertex_labels = IO.read_vertex_labels(args.Rvlabels_file)

    Ltriangles, Lvertices, Lvertex_coord, Ltriangles_vertex = IO.read_mesh_vtk(args.LVtk_file,Lvertex_labels)
    Ltriangles =  label_triangles(Ltriangles)  
    Rtriangles, Rvertices, Rvertex_coord, Rtriangles_vertex = IO.read_mesh_vtk(args.RVtk_file, Rvertex_labels)
    Rtriangles = label_triangles(Rtriangles)

    Lanatomic_parcels,Lparcel_names = IO.read_intersection(args.Intersection_dir, Lparcel_names,Ltriangles,Lanatomic_parcels,'l') 
    Ranatomic_parcels, Rparcel_names = IO.read_intersection(args.Intersection_dir, Rparcel_names, Rtriangles,Ranatomic_parcels, 'r')

    pct = args.pct 
    thr_prob = args.density_center 
    size_thr = args.size_thr 

    if trac == "y":
        update_fibers_map(Lanatomic_parcels)
        for aparcel in Lanatomic_parcels:
            recalc_probability(aparcel)
        for aparcel in Ranatomic_parcels:
            recalc_probability(aparcel)
        IO.write_atlas(Lanatomic_parcels, Lparcel_names,Ltriangles, trac_path,True,"L")
        IO.write_atlas(Ranatomic_parcels, Rparcel_names,Rtriangles, trac_path,True,"R")

    processing_parcels(Lanatomic_parcels,pct,thr_prob,size_thr,Lparcel_names,trac,trac_path,"L")
    processing_parcels(Ranatomic_parcels,pct,thr_prob,size_thr,Rparcel_names,trac,trac_path,"R")
    get_hard_parcels(Lanatomic_parcels)
    get_hard_parcels(Ranatomic_parcels)

    IO.write_atlas(Lanatomic_parcels,Lparcel_names,Ltriangles,atlas_path,False,"L")
    IO.write_atlas(Ranatomic_parcels,Rparcel_names,Rtriangles,atlas_path,False,"R")

    nLparcels = len([name for name in Lparcel_names if name != "None"])
    nRparcels = len([name for name in Rparcel_names if name != "None"])
    nLhparcels = 0
    for i,ap in enumerate(Lanatomic_parcels):
        for hp in ap.hard_parcels:
            if len(hp.triangles) > 0:
                nLhparcels+=1
    nRhparcels = 0
    for i,ap in enumerate(Ranatomic_parcels):
        for hp in ap.hard_parcels:
            if len(hp.triangles) > 0:
                nRhparcels += 1

    import os
    duration = 1 
    freq = 440
    os.system('play -nq -t alsa synth {} sine {}'.format(duration, freq))
if __name__ == '__main__':
    main()
