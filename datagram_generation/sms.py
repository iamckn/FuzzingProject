#!c:\\python\\python.exe

#
# pedram amini <pamini@tippingpoint.com>
#
# this was a really half assed fuzz. someone should take it further, see my notes in the requests file for more info.
#

from sulley   import *
#from requests import smil
import sys
import time
import fuzzutils as ft
msg_pool=[]

########################################################################################################################

class connection (pgraph.edge.edge):
    def __init__ (self, src, dst, callback=None):
        pgraph.edge.edge.__init__(self, src, dst)
        self.callback = callback

class session_file (pgraph.graph):
    def __init__ (self, session_filename=None, skip=0, sleep_time=1.0, log_level=2, proto="tcp", restart_interval=0, timeout=5.0, web_port=26000, crash_threshold=3):
        pgraph.graph.__init__(self)
        self.root       = pgraph.node()
        self.root.name  = "__ROOT_NODE__"
        self.root.label = self.root.name
        self.last_recv  = None
        self.add_node(self.root)

    def log (self, msg, level=1):
        print "[%s] %s" % (time.strftime("%I:%M.%S"), msg)
    def add_node (self, node):
        node.number = len(self.nodes)
        node.id     = len(self.nodes)
        if not self.nodes.has_key(node.id):
            self.nodes[node.id] = node
        return self
    def connect (self, src, dst=None, callback=None):
        if not dst:
            dst = src
            src = self.root
        if type(src) is str:
            src = self.find_node("name", src)
        if type(dst) is str:
            dst = self.find_node("name", dst)
        if src != self.root and not self.find_node("name", src.name):
            self.add_node(src)
        if not self.find_node("name", dst.name):
            self.add_node(dst)
        edge = connection(src.id, dst.id, callback)
        self.add_edge(edge)
        return edge
    def num_mutations (self, this_node=None, path=[]):
        if not this_node:
            this_node                = self.root
            self.total_num_mutations = 0
        for edge in self.edges_from(this_node.id):
            next_node                 = self.nodes[edge.dst]
            self.total_num_mutations += next_node.num_mutations()
            if edge.src != self.root.id:
                path.append(edge)
            self.num_mutations(next_node, path)
        if path:
            path.pop()
        return self.total_num_mutations
    def server_init (self):
        self.total_mutant_index  = 0
        self.total_num_mutations = self.num_mutations()
    def fuzz (self, this_node=None, path=[]):
        if not this_node:
            if not self.edges_from(self.root.id):
                raise sex.error("NO REQUESTS SPECIFIED IN SESSION")
            this_node = self.root
            try:    self.server_init()
            except: return
        for edge in self.edges_from(this_node.id):
            self.fuzz_node = self.nodes[edge.dst]
            num_mutations  = self.fuzz_node.num_mutations()
            if edge.src != self.root.id:
                path.append(edge)
            current_path  = " -> ".join([self.nodes[e.src].name for e in path])
            current_path += " -> %s" % self.fuzz_node.name
            self.log("current fuzz path: %s" % current_path, 2)
            self.log("fuzzed %d of %d total cases" % (self.total_mutant_index, self.total_num_mutations), 2)
            done_with_fuzz_node = False
            crash_count         = 0
            # loop through all possible mutations of the fuzz node.
            while not done_with_fuzz_node:
                if not self.fuzz_node.mutate():
                    self.log("all possible mutations for current fuzz node exhausted", 2)
                    done_with_fuzz_node = True
                    continue
                self.total_mutant_index += 1
                # if we don't need to skip the current test case.
                self.log("fuzzing %d of %d" % (self.fuzz_node.mutant_index, num_mutations), 2)
                while 1:
                    output = ""
                    for e in path:
                        node = self.nodes[e.src]
                        output += node.render()
                    output += self.fuzz_node.render()
                    break
                self.render_rest(self.fuzz_node, output)
            temp = self.nodes[edge.dst]
            self.fuzz(self.fuzz_node, path)
            temp.reset()

        # finished with the last node on the path, pop it off the path stack.
        if path:
            path.pop()

    def render_rest(self, this_node=None, output=""):
            original_output = output
            if(len(self.edges_from(this_node.id)) > 0):
                for edge in self.edges_from(this_node.id):
                    output = original_output
                    output += self.nodes[edge.dst].render()
                    self.render_rest(self.nodes[edge.dst], output)
            else:
                print output
                msg_pool.append((output,len(output)))


def eight_bit_encoder(string):
        ret = ''
        strlen = len(string)
        for i in range(0,strlen):
                temp = "%02x" % ord(string[i])
                ret += temp.upper()
        return ret


s_initialize("query")

##s_size("from_number", format="oct", length=1, math=lambda x: x-3)
##if s_block_start("from_number"):
##       s_byte(0x91, format="oct", name="typeofaddress_from")
##       if s_block_start("abyte2", encoder=eight_bit_encoder):
##               s_string("\x94\x71\x96\x46\x66\x56\xf8", max_len = 128)
##       s_block_end()
##s_block_end()
##
##
##s_size("SMSC_number", format="oct", length=1, math=lambda x: x/2)
##if s_block_start("SMSC_number"):
##        s_byte(0x91, format="oct", name="typeofaddress")
##        if s_block_start("SMSC_data", encoder=eight_bit_encoder):
##                s_string("\x94\x71\x06\x00\x40\x34", max_len = 254)
##        s_block_end()
##s_block_end()

s_byte(0x04, format="oct", name="octetofsmsdeliver",fuzzable=False)

s_size("from_number", format="oct", length=1, math=lambda x: x-3)
if s_block_start("from_number"):
       s_byte(0x91, format="oct", name="typeofaddress_from")
       if s_block_start("abyte2", encoder=eight_bit_encoder):
               s_string("\x94\x71\x96\x46\x66\x56\xf8", max_len = 128)
       s_block_end()
s_block_end()
##s_size("refrom_number", format="oct", length=1, math=lambda x: x-3)
##if s_block_start("refrom_number"):
##       s_byte(0x91, format="oct", name="typeofaddress_dst")
##       if s_block_start("abyte3", encoder=eight_bit_encoder):
##               s_string("\x94\x71\x96\x46\x66\x56\xf8", max_len = 128)
##       s_block_end()
##s_block_end()


s_byte(0x0, format="oct", name="tp_pid",fuzzable=False)
s_byte(0x04, format="oct", name="tp_dcs",fuzzable=False)

if s_block_start("date"):
                s_byte(0x90, format="oct")
                s_byte(0x10, format="oct")
                s_byte(0x82, format="oct")
                s_byte(0x11, format="oct")
                s_byte(0x42, format="oct")
                s_byte(0x15, format="oct")
                s_byte(0x40, format="oct")
s_block_end()

if s_block_start("eight_bit"):
        s_size("message_eight", format="oct", length=1, math=lambda x: x / 2, fuzzable=True)
        if s_block_start("message_eight"):
                if s_block_start("text_eight", encoder=eight_bit_encoder):
                        s_string("hellohello", max_len = 256)
                s_block_end()
        s_block_end()
s_block_end()

fuzz_file = session_file()
fuzz_file.connect(s_get("query"))
fuzz_file.fuzz()

ft.cases2file(msg_pool,'sulley.cases')

