# -*- coding: utf-8 -*-
"""
Created on Thu Oct 11 21:15:23 2018

@author: felipe

Last update 08/01/2019 Narciso

"""
import os
import subprocess as sp
import shutil
import sys
import time

def main():
	Lhemi_path = sys.argv[1]
	Rhemi_path = sys.argv[2]
	Lbundles_path = sys.argv[3]
	Rbundles_path = sys.argv[4]
	output_path = sys.argv[5]

	Lintersection_path = output_path + '/left-hemi' 
	Rintersection_path = output_path + '/right-hemi'

	if os.path.exists(output_path):
		shutil.rmtree(output_path)

	os.makedirs(Lintersection_path)
	os.makedirs(Rintersection_path)

	sp.call(['make'])
	start = time.time()
	sp.call(['./main', Lhemi_path, Rhemi_path, Lbundles_path, Rbundles_path, Lintersection_path, Rintersection_path])
	end = time.time()
	print(end - start)

if __name__=="__main__":
	main()  
