from __future__ import print_function
import sys
import csv
from collections import defaultdict
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


superblock_list = defaultdict(list)
group_list = defaultdict(list)
inode_info = []
block_list = defaultdict(list)
free_blocks = []
free_inodes = []
inode_array = defaultdict(list)
dirents = []
idirects = []
valid_inodes = []

inode_num_array = []

inode_links = defaultdict(list)
inode_links_counter = 0
dirent_links = defaultdict(list)
dirent_counter = 0

block_array = {}
parent_array = {}
parent_array_counter = 0

count_blocks = 0
block_num = 0
first_valid_block = 0
block_size = 0
inode_size = 0
total_blocks = 0
total_inodes = 0
inode_count = 0
first_non_reserved = 0
total_num_blocks = 0


def superblock_parse(row):
	global block_size
	global inode_size
	global first_non_reserved
	global total_num_blocks

	block_size = int(row[3])
	superblock_list[0].append(block_size)
	inode_size = int(row[4])
	superblock_list[1].append(inode_size)
	total_num_blocks = int(row[2])
	superblock_list[2].append(total_num_blocks)
	first_non_reserved = int(row[7])
	superblock_list[3].append(first_non_reserved)

def group_parse(row):
	global first_valid_block
	global total_inodes 
	global total_blocks

	total_blocks = int(row[2])
	group_list[0].append(total_blocks)

	total_inodes = int(row[3])
	group_list[1].append(total_inodes)


	first_valid_block = int(row[8]) + inode_size*total_inodes / block_size
	#print("first_valid_block: {}".format(first_valid_block))


def block_consistency_audits():
	global count_blocks
	global block_num
	global free_blocks
	global total_blocks
	checking_block_list = []

	for block in range(0,count_blocks):
		#print(block)
		block_num = block_list[block][0]
		checking_block_list.append(block_num[0])

		if block_num[0] < 0 or block_num[0] > total_blocks:
			print('INVALID ' + str(block_num[1]) + 'BLOCK ' + str(block_num[0]) + ' IN INODE ' + str(block_num[2]) + ' AT OFFSET ' + str(block_num[3]) )
		elif block_num[0] > 0 and  block_num[0] < first_valid_block:
			print('RESERVED ' + str(block_num[1]) + 'BLOCK ' + str(block_num[0]) + ' IN INODE ' + str(block_num[2]) + ' AT OFFSET ' + str(block_num[3]) )

	for block in range(8,total_blocks):
		if block not in free_blocks and block not in checking_block_list and block not in idirects:
			print('UNREFERENCED BLOCK {}'.format(block))
		if block in free_blocks and (block in checking_block_list or block in idirects):
			print('ALLOCATED BLOCK {} ON FREELIST'.format(block))


	for i in range(0,total_blocks):
		for j in range(i+1, total_blocks):
			block_i = block_list[i][0]
			block_j = block_list[j][0]

			# if block_i[0] != 0 and block_j[0] != 0:
			# 	print("{}:{}".format(block_i[0], block_j[0]))

			if block_i[0] == block_j[0] and (block_i[0] >7 and block_j[0] > 7):
				# print("i: {} and j: {}".format(str(i), str(j)))
				print("DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(str(block_i[1]), str(block_i[0]), str(block_i[2]), str(block_i[3])))
				print("DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(str(block_j[1]), str(block_j[0]), str(block_j[2]), str(block_j[3])))


def inode_allocation_audit():
	global free_inodes
	global total_inodes
	global inode_count
	global inode_array
	global first_non_reserved
	global total_num_blocks
	global valid_inodes

	for i in range(0,inode_count):
		inode = inode_array[i][0]
		# print(inode)
		#print(free_inodes)
		if inode[0] in free_inodes and inode[1] == 1:
			print("ALLOCATED INODE {} ON FREELIST".format(inode[0]))


	for i in range(first_non_reserved, total_num_blocks+1):
		#print("valid_inodes free_inodes i: {} {} {} ".format(valid_inodes, free_inodes, i))
		if i not in valid_inodes and i not in free_inodes:
				print("UNALLOCATED INODE {} NOT ON FREELIST".format(i))


def inode_parse(row):
	global count_blocks
	global total_blocks
	global inode_count
	global inode_links_counter
	global inode_links
	validity = 0
	no_inode = int(row[1])

	#starting of all indirect nodes
	for i in range(12,27):

		if i == 24 and int(row[i]) != 0:
			level = 'INDIRECT '
			block_offset = 12
		elif i == 25 and int(row[i]) != 0:
			level = 'DOUBLE INDIRECT '
			block_offset = 268
		elif i == 26 and int(row[i]) != 0:
			level = 'TRIPLE INDIRECT '
			block_offset = 65804
		else:
			level = ''
			block_offset = 0

		block_index = int(row[i])

		inode_info = [block_index, level, no_inode, block_offset]
		
		block_list[count_blocks].append(inode_info)
		count_blocks = count_blocks + 1



	#print("inode_num link_count mode {} {} {}".format(no_inode, row[8], row[5]))
		#(row[2] == 'd' or row[2] == 'f' or row[2] == 's')
	if row[8] > 0 and row[5] > 0:
		validity = 1
		valid_inodes.append(no_inode)
		if row[2] == 'd' or row[2] == 'f':
			link_info = [int(row[1]),int(row[6])]
			inode_links[inode_links_counter].append(link_info)
			inode_links_counter = inode_links_counter + 1


	validity_info = [no_inode, validity]
	inode_array[inode_count].append(validity_info)
	inode_count = inode_count + 1

def directory_consistency_audit():
	global inode_links
	global dirent_links
	global inode_links_counter
	global dirent_counter
	global inode_count
	global total_inodes
	global valid_inodes
	found = False
	unalloc_bool = False

	for i in range(0, inode_links_counter):
		i_link = inode_links[i][0]
		found = False
		for j in range(0, dirent_counter):
			d_link = dirent_links[j][0]

			if i_link[0] == d_link[0]:
				found = True
				if i_link[1] != d_link[1]:
					print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(i_link[0],  d_link[1],i_link[1]))
		if found == False:
			print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(i_link[0], 0 ,i_link[1]))


	for i in range(0, dirent_counter):
		d_link = dirent_links[i][0]
		for j in range(0, inode_count):
			inode = inode_array[j][0]
			if d_link[2] ==  inode[0]:
				# print("dlink: {}".format(d_link))
				# print("inode: {}".format(inode))
				if d_link[0] < 1 or d_link[0] > total_inodes: 
					print("DIRECTORY INODE {} NAME 'bogusEntry' INVALID INODE {}".format(inode[0], d_link[0]))
					unalloc_bool = True
		if d_link[0] not in valid_inodes and (unalloc_bool == False):
				print("DIRECTORY INODE {} NAME 'nullEntry' UNALLOCATED INODE {}".format(d_link[2], d_link[0]))
			

def dirent_parse(row):
	global inode_links_counter
	global dirent_counter
	global dirent_links
	global parent_array
	global parent_array_counter

	ref_inode = int(row[3])
	parent_inode = int(row[1])
	dirent_info = [ref_inode, 1, parent_inode]

	found = False

	if dirent_counter > 0:
		for i in range(0, dirent_counter):
			
			if ref_inode == dirent_links[i][0][0]:
				
				found = True
				dirent_links[i][0][1] = dirent_links[i][0][1] + 1

		if found == False:
			dirent_links[dirent_counter].append(dirent_info)
			# print(dirent_links)
			dirent_counter = dirent_counter + 1
	else:
		dirent_links[dirent_counter].append(dirent_info)
		dirent_counter = dirent_counter + 1

	if ref_inode not in parent_array:
		parent_array[ref_inode] = parent_inode

	if str(row[6]) == "'.'":
		# parent = [parent_inode, ref_inode]
		# parent_array[parent_array_counter] = parent
		#print("parent: {}".format(parent_array))
		# parent_array_counter = parent_array_counter + 1
		if(parent_inode != ref_inode):
			print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(str(parent_inode), str(ref_inode), str(parent_inode)))

	
	
	if str(row[6]) == "'..'":
		# for i in range(0, parent_array_counter):
			# parent = parent_array[i]
			#print(parent)

		if ref_inode != parent_array[parent_inode]:
			print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(str(parent_inode), str(ref_inode), str(parent_inode)))

			


def indirect_parse(row):
	global count_blocks

	indirect_node_num = int(row[5])
	idirects.append(indirect_node_num)
	if int(row[2]) == 1:
		level = 'INDIRECT '
		block_offset = 12
	if int(row[2]) == 2:
		level = 'DOUBLE INDIRECT '
		block_offset = 268
	if int(row[2]) == 3:
		level = 'TRIPLE INDIRECT '
		block_offset = 65804

	no_inode = int(row[1])
	inode_info = [indirect_node_num, level, no_inode, block_offset]
	block_list[count_blocks].append(inode_info)
	count_blocks = count_blocks + 1


def bfree_parse(row):
	free_block_num = int(row[1])
	free_blocks.append(free_block_num)

def ifree_parse(row):
	free_inode_num = int(row[1])
	free_inodes.append(free_inode_num)


def main():
	if len(sys.argv) != 2:
		sys.stderr.write('Error: wrong number of arguments\n')
		sys.exit(1)

	try:
		with open(sys.argv[1], 'r') as input_file:
			parser = csv.reader(input_file)
			#print("loaded file")
			for row in parser:
				#print(row)
				if row[0] == 'SUPERBLOCK':
					superblock_parse(row)
					#print("finished superblock_parse")
				elif row[0] == 'GROUP':
					group_parse(row)
					#print("finished group_parse")
				elif row[0] == 'BFREE':
					bfree_parse(row)
					#print("finished bfree_parse")
				elif row[0] == 'IFREE':
					ifree_parse(row)
					#print("finished ifree_parse")
				elif row[0] == 'INODE':
					inode_parse(row)
					#print("finished inode_parse")
				elif row[0] == 'DIRENT':
					dirent_parse(row)
					#print("finished dirent_parse")
				elif row[0] == 'INDIRECT':
					indirect_parse(row)
					#print("finished indirect_parse")
	except(IOError):
			eprint("IOError detected.")
			exit(1)

	block_consistency_audits()
	inode_allocation_audit()
	directory_consistency_audit()
	#print("finished block_consistency_audits")


if __name__== "__main__":
  main()