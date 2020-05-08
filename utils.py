
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
#Creation date: 31/01/2018
#Last update: 20/07/2019


import operator
import numpy as np
import collections

def find_label(name,parcel_names):
    for i,n in enumerate(parcel_names):
        if n == name:
            return i
    return -1

def most_common(lst):
    return max(set(lst), key=lst.count)

def most_probable(map):
    if (len(map)!=0):
        return max(map.items(), key=operator.itemgetter(1))[0]
    else:
        return -1

def labels_by_prob(map,prob):
    labels = []
    for key,value in map.items():
        if (value < prob):
            labels.append(key)
    return labels

def fusion_names(n1,n2):
    index = n1.split("_")[1]
    name1_split = n1.split("_")[0].split("-")
    name2_split = n2.split("_")[0].split("-")
    new_name = ""
    for i,name1 in enumerate(name1_split):
        if i == 0:
            new_name += name1
        else:
            new_name+="-"+ name1
    for i,name2 in enumerate(name2_split):
        if i > 0:
            new_name+="-"+name2
    new_name+="_"+index
    return new_name

def intersection(lst1, lst2):
    temp = set(lst2)
    lst3 = [value for value in lst1 if value in temp]
    return sorted(lst3, key=lambda tri: tri.index)

def union(lst1, lst2):
    final_list = list(set(lst1) | set(lst2))
    return final_list

def get_coords(coords,index):
    fiber_coords = []
    for i in range(0,63,3):
        fiber_coords.append(np.asarray([coords[63*index+i], coords[63*index+i+1], coords[63*index+i+2]]))
    return (np.asarray(fiber_coords))

def get_inter_coords(coords,index):
    return np.asarray([coords[3*index], coords[3*index+1], coords[3*index+2]])

def sort_graphs(dic):
    dic = collections.OrderedDict(dic)
    return dict(sorted(dic.items(), key=lambda kv: len(kv[1])))

def map_triangles(triangles):
    map_tri = {}
    for tri in triangles:
        alabel = tri.label_parcel
        map_tri[alabel] = [tri]
    return map_tri