
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


import numpy as np
import os
from classes import *
import utils
import shutil
import bundleTools
import itertools


def create_atlas_dirs(path,trac):
    trac_path = ""
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path)
    atlas_path = path+"/final_atlas"
    os.mkdir(atlas_path)
    if trac.lower() == "y":
        trac_path = path+"/trac_atlas"
    os.mkdir(trac_path)
    connect_path = path+"/connectogram"
    os.mkdir(connect_path)
    return atlas_path,trac_path,connect_path


def read_parcel_names(names_path):
    parcel_names = []
    parcel_lnames = []
    anatomic_parcels = []
    with open(names_path, 'r') as file:
        for i,line in enumerate(file):
            spplited = line.split(" ")
            parcel_names.append(str(spplited[1].rstrip("\n\r")))
            parcel_lnames.append(str(spplited[0].rstrip("\n\r")))
            anatomicParcel = AnatomicParcel(i)
            anatomic_parcels.append(anatomicParcel)
    return(parcel_lnames, parcel_names, anatomic_parcels)

def read_vertex_labels(labels_path):
    labels = []
    with open(labels_path, 'r') as file:
        for line in file:
            label = int(line)
            if label == -1:
                label = 0
            labels.append(label)
    return(np.asarray(labels))


def read_mesh_vtk(mesh_path,vertex_labels):
    vertices, triangles, vertex_coord, triangles_vertices = [],[],[],[]
    with open(mesh_path) as file:
        lines = file.readlines()
        vertex_index = 0
        triangle_index = 0
        for line in lines:
            splitted = line.split(" ")
            id = str(splitted[0])
            if (id == 'v'):
                label = vertex_labels[vertex_index]
                vertex = Vertex(vertex_index,float(splitted[1]),float(splitted[2]),float(splitted[3]),label,[])
                vertex_index+=1
                vertices.append(vertex)
                coords = np.asarray([float(splitted[1]), float(splitted[2]), float(splitted[3])])
                vertex_coord.append(coords)
            if (id == 'f'):
                id_v1 = int(splitted[1].split("//")[0])
                id_v2 = int(splitted[2].split("//")[0])
                id_v3 = int(splitted[3].split("//")[0])
                v1 = vertices[id_v1]    
                v2 = vertices[id_v2]
                v3 = vertices[id_v3]
                triangle = Triangle(triangle_index,v1,v2,v3,-1,[],[])  
                triangles.append(triangle)  
                v1.triangles.append(triangle)   
                v2.triangles.append(triangle)
                v3.triangles.append(triangle)
                triangle_index+=1
                vs = np.asarray([int(splitted[1].split("//")[0]) - 1, int(splitted[2].split("//")[0]) - 1,
                                int(splitted[3].split("//")[0]) - 1])
                triangles_vertices.append(vs)
    file.close()
    return np.asarray(triangles),np.asarray(vertices),np.asarray(vertex_coord),np.asarray(triangles_vertices);



def read_intersection_file(infile,subject_name,name,parcel_names,triangles,anatomic_parcels):
    with open(infile) as f:
        content = f.readlines()
        if (content[0] != '0\n'):
            bundle_name = (name.split("_")[1]).split(("-"))
            bundle_index = str((name.split("_")[2]).split(".")[0])  

            anatomic_label1 = utils.find_label(bundle_name[0],parcel_names)
            anatomic_label2 = utils.find_label(bundle_name[1],parcel_names)

            anatomic_parcel1 = anatomic_parcels[anatomic_label1]
            anatomic_parcel2 = anatomic_parcels[anatomic_label2]

            if (bundle_name[0] != bundle_name[1]):
                parcel1_name = bundle_name[0]+"_"+bundle_name[1]+"_"+str(bundle_index)
                parcel2_name = bundle_name[1]+"_"+bundle_name[0]+"_"+str(bundle_index)
            else:
                parcel1_name = bundle_name[0] + "0-" + bundle_name[1] + "1_" + str(bundle_index)
                parcel2_name = bundle_name[1] + "1-" + bundle_name[0] + "0_" + str(bundle_index)


            parcel1_label = utils.find_label(parcel1_name,parcel_names)
            parcel2_label = utils.find_label(parcel2_name,parcel_names)
            parcel_names.append(parcel1_name)   
            parcel_names.append(parcel2_name)
            parcel1_label = len(parcel_names) - 1 
            parcel2_label = len(parcel_names) 
            parcel1 = SubParcel(parcel1_label,anatomic_label1,[],[])
            parcel2 = SubParcel(parcel2_label,anatomic_label2,[],[])
            parcel1.connected_parcels.append(parcel2)
            parcel2.connected_parcels.append(parcel1)
            anatomic_parcel1.sub_parcels.append(parcel1)  
            anatomic_parcel2.sub_parcels.append(parcel2)

            fibers = content[5].split(" ")
            triangles_init = content[1].split(" ")
            triangles_end = content[2].split(" ")
            inter_inits = list(map(np.float32,content[3].split(" ")))
            inter_ends = list(map(np.float32,content[4].split(" ")))

            for i in range(len(fibers)):
                triangle_init = triangles[int(triangles_init[i])]
                triangle_end = triangles[int(triangles_end[i])]
                inter_init = utils.get_inter_coords(inter_inits,i)
                inter_end = utils.get_inter_coords(inter_ends,i)
                fiber_coords = []
                fiber = Fiber(int(fibers[i]),subject_name,triangle_init,triangle_end,fiber_coords,inter_inits[i],inter_ends[i])
                parcel1.fibers.append(fiber) 
                parcel2.fibers.append(fiber)
                triangle_init.fibers.append(fiber)
                triangle_end.fibers.append(fiber)

                triangle_init.labels_subparcel.append(parcel1.label)
                triangle_end.labels_subparcel.append(parcel2.label)
                if triangle_init not in parcel1.triangles:
                    parcel1.triangles.append(triangle_init)
                    parcel1.inter_points.append(inter_init)
                if triangle_end not in parcel2.triangles:
                    parcel2.triangles.append(triangle_end)
                    parcel2.inter_points.append(inter_end)

    return anatomic_parcels, parcel_names


def read_intersection (intersection_path,parcel_names,triangles,anatomic_parcels,hemi):
    if hemi == 'l':
        selected_hemi = "left-hemi"
    else:
        selected_hemi = "right-hemi"
    subject_dirs = os.listdir(intersection_path)
    subject_dirs.sort()
    for subj_dir in subject_dirs:
        subject_name = subj_dir.split("-")[0]
        intersectionDir = os.listdir(intersection_path+"/"+subj_dir+"/"+subj_dir.split("_")[0]+"_"+selected_hemi)
        intersectionDir.sort()
        for file in intersectionDir:
            file_path = intersection_path+"/"+subj_dir+"/"+subj_dir.split("_")[0]+"_"+selected_hemi+"/"+file
            anatomic_parcels,parcel_names = read_intersection_file(file_path,subject_name,file,parcel_names,triangles,anatomic_parcels)
    return anatomic_parcels,parcel_names



def rename_bundles(segmentation_path):
    subject_dirs = os.listdir(segmentation_path)
    subject_dirs.sort()
    for subj_dir in subject_dirs:
        for hemi in ["left-hemi", "right-hemi"]:
            segmentationDir = os.listdir(segmentation_path+"/"+subj_dir+"/"+hemi)
            segmentationDir.sort()
            for file in segmentationDir:
                file_split = file.split("-")
                if (len(file_split)>3):
                    index = file_split[3].split(".")[0]
                    extension = file_split[3].split(".")[1]
                    new_name = file_split[0]+"-"+file_split[1]+"-"+file_split[2]+"_"+index+"."+extension
                    original_path = segmentation_path+"/"+subj_dir+"/"+hemi+"/"+file
                    new_path = segmentation_path+"/"+subj_dir+"/"+hemi+"/"+new_name
                    os.rename(original_path, new_path)


def write_atlas_names(names,atlas_path,hemi):
    file_path = atlas_path+"/"+hemi+"atlas_names.txt"
    f = open(file_path, "w+")
    for i,n in enumerate(names):
        if n != "None":
            f.write(str(i)+" "+n+"\n")
    f.close()

def write_parcels(aparcels,atlas_path,hemi):
    file_path = atlas_path+"/"+hemi+"parcels.txt"
    f = open(file_path,"w+")
    for ap in aparcels:
        f.write("ap "+str(ap.label)+"\n")
        for sp in ap.sub_parcels:
            f.write("sp " + str(sp.label) + "\n")  
            f.write("c")
            for cp in sp.connected_parcels:
                f.write(" "+str(cp.label))
            f.write("\n")
            f.write("t")
            for t in sp.triangles:  
                f.write(" "+str(t.index))
            f.write("\n")
            f.write("p")
            for t in sp.triangles: 
                prob  = round(t.prob_map[sp.label],5)
                f.write(" "+str(prob))
            f.write("\n")
    f.close()


def write_hparcels(aparcels,triangles,atlas_path,names,hemi):
    tri_map = utils.map_triangles(triangles)
    file_path = atlas_path+"/"+hemi+"hard_parcels.txt"
    f = open(file_path,"w+")
    new_hlabel = len(names)
    for ap in aparcels:
        ap_alltri = []
        ap_triangles = []
        if ap.label in tri_map:
            ap_alltri = tri_map[ap.label]
        f.write("ap "+str(ap.label)+"\n") 
        if len(ap.hard_parcels) > 0:
            for hp in ap.hard_parcels:
                if len(hp.triangles)>0:
                    f.write("hp " + str(hp.label) + "\n") 
                    f.write("t")
                    for t in hp.triangles:  
                        ap_triangles.append(t)
                        f.write(" "+str(t.index))
                    f.write("\n")
        else:
            f.write("hp " + str(new_hlabel) + "\n") 
            for tri in ap_alltri:
                f.write(" " + str(tri.index)) 
            new_hlabel+=1
            f.write("\n")

    f.close()

def select_coords(fibers):
    map_coords = {}
    for f in fibers:
        if f.subject not in map_coords:
            map_coords[f.subject] = [f.coords]
        else:
            map_coords[f.subject].append(f.coords)

    if "001" not in map_coords:
        for key,value in map_coords.items():
            if len(value) != 0:
                selected_fibers = value
                break
    else:
        selected_fibers = map_coords["001"]

    return np.asarray(selected_fibers)


def write_bundles(aparcels,apath,hemi,names):
    bundles_path = apath+"/"+hemi+"bundles"
    os.mkdir(bundles_path)
    if hemi == "L":
        prefix = "lh"
    else:
        prefix = "rh"
    for ap in aparcels:
        for sp in ap.sub_parcels:
            bundle_name = prefix+"_"+names[sp.label]+".txt"
            file = open(bundles_path+"/"+bundle_name,"w+")
            for tri in sp.triangles:
                if sp.label in tri.fibers_map:
                    fibers = tri.fibers_map[sp.label]
                    coords = select_coords(fibers)
                    file.write(str(tri.index)+"\n")
                    for coord in coords:
                        coord = list(itertools.chain.from_iterable(coord))
                        file.write("".join([str(c) + "~" for c in coord]))
                    file.write("\n")

            file.close()

def write_k(aparcels,path,hemi):
    file_path = path+"/"+hemi+"k.txt"
    file = open(file_path,"w+")
    for ap in aparcels:
        label = str(ap.label)
        k=0
        for hp in ap.hard_parcels:
            if len(hp.triangles)>0:
                k+=1
        file.write(label+" "+str(k)+"\n")
    file.close()


def write_atlas(anatomic_parcels,names,triangles,atlas_path,trac,hemi):
    if trac == False:
        write_k(anatomic_parcels,atlas_path,hemi)
        write_hparcels(anatomic_parcels,triangles,atlas_path,names,hemi)

    write_atlas_names(names,atlas_path,hemi)
    write_parcels(anatomic_parcels,atlas_path,hemi)

