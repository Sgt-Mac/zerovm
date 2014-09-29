#! /usr/bin/env python

import sys
import socket
import signal
import struct

################################################################################
## Utility Signal handler
################################################################################
__callback_func__ = None

######
def __on_exit__( signal_type, frame):
  global __callback_func__
  if( __callback_func__ != None):
    __callback_func__( signal_type)
  raise SystemExit()

######
def set_exit_handler( func):
  global __callback_func__

  __callback_func__ = func

  signal.signal( signal.SIGTERM, __on_exit__)

######
def signal_numtoname (num):
  retval = "%d" % num

  name = [ key for key in signal.__dict__ if num == signal.__dict__[key] ]

  if( name is not None):
    retval = name[0]

  return( retval)


######
def on_exit( sig):
  sig_name = signal_numtoname( sig)
  sys.stdout.write( "[!] Exit Handler Triggered!! Signal: [%s](%s)\n" % (sig, sig_name))


################################################################################
# a generic hex dump
################################################################################
def my_hex_dump( data, address=0, byte_grouping=4, bytes_per_line=16, prefix="", address_align=False):
  '''
  Utility function that converts data into hex dump format.

  @type  data          : Raw Bytes
  @param data          : Raw bytes to view in hex dump
  @type  address       : DWORD
  @param address       : Address to start hex offset display from (Optional, def=0)
  @type  byte_grouping : DWORD
  @param byte_grouping : Number of bytes to be grouped together (Optional, def=4)
  @type  bytes_per_line: DWORD
  @param bytes_per_line: Number of bytes to display on a line (Optional, def=16)
  @type  prefix        : String
  @param prefix        : String to prefix each line of hex dump with (Optional, def="")
  @type  address_align : Boolean
  @param address_align : Align relative to bytes_per_line (Optional, def=False)

  @rtype:  String
  @return: Hex dump of data.
  '''

  # Address Align == True
  # ADDRESS :  H E X  D U M P                                  ASCII DUMP
  # 00000005:                00 00 00 00 00 00 00 00 00 00 00       ...........
  # 00000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  # 00000020: 00 00                                            ..

  # Address Align == False
  # ADDRESS :  H E X  D U M P                                  ASCII DUMP
  # 00000005: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00       ...........
  # 00000015: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
  # 00000025: 00 00 00 00 00 00                                ......

  retval = ""

  if address >= 0 and byte_grouping > 0 and bytes_per_line > 0:
    # Setup Hex and Ascii Dump...
    hexdump   = ""
    asciidump = ""
    bytecount = 0
    alignbytes = 0

    # Align relative to bytes_per_line
    if( address_align == True):
      alignbytes = address%bytes_per_line
      if( alignbytes != 0):
        #address  -= alignbytes
        padlen    = (alignbytes*2) + (alignbytes/byte_grouping)
        hexdump   = " "*padlen
        asciidump = " "*alignbytes
        bytecount = alignbytes

    for byte in data:
      bytecount += 1

      # Setup Hex Dump
      hexdump += "%02x" % ord( byte)
      if( (bytecount % byte_grouping) == 0):
        hexdump += " "

      # Setup Ascii Dump
      if( byte >= ' ' and byte <= '~'):
        asciidump += byte
      else:
        asciidump += "."

      # Add Address, Hex Dump, and Ascii Dump to Line/Ret Dump
      if (bytecount % bytes_per_line) == 0:
        retval   += "%s%08x: %s %s\n" % (prefix, address, hexdump, asciidump)

        if( bytecount == bytes_per_line):
          address -= alignbytes

        address  += bytes_per_line
        hexdump   = ""
        asciidump = ""

    # Align last line...
    bytecount  = len( asciidump)
    deltabytes = bytes_per_line - bytecount
    padlen     = (deltabytes*2) + (deltabytes/byte_grouping)
    if( (deltabytes%byte_grouping) != 0):
      padlen += 1
    hexdump += " "*padlen

    # Add Address, Hex Dump, and Ascii Dump to Last Line/Ret Dump
    if( bytecount > 0 ):
      retval += "%s%08x: %s %s\n" % (prefix, address, hexdump, asciidump)

  return( retval)





################################################################################
## Generic UDP Echo Server
################################################################################
def udp_echo_server( server_addr, port):

  sock_fd = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
  sock_fd.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  sock_fd.bind( (server_addr, port))

  sys.stdout.write( "[*] UDP Echo Server is up and listening on: [%s]:[%d]\n" %
      (server_addr, port));

  while( True):
    try:
      num_bytes, raddr = sock_fd.recvfrom( 1);

      num_bytes = struct.unpack( ">B", num_bytes)[0]

      data, raddr = sock_fd.recvfrom( num_bytes);
      sys.stdout.write( "\n|%s|\n" %("="*78 ))
      sys.stdout.write( "[*] Recv'd: [%d] bytes from [%s]:[%d]\n" %
          ( num_bytes, raddr[0], raddr[1]))
      sys.stdout.write( "%s\n" % (my_hex_dump( data)))


      sys.stdout.write( "[*] sending: [%d] bytes to [%s]:[%d]\n" %
          ( len( data), raddr[0], raddr[1]))
      sys.stdout.write( "%s\n" % (my_hex_dump( data)))
      num_bytes = struct.pack( ">B", len( data))
      retval = sock_fd.sendto( num_bytes, raddr);
      retval = sock_fd.sendto( data, raddr);


    except KeyboardInterrupt:
      sock_fd.close()
      raise KeyboardInterrupt()


################################################################################
##
## Main Entry Point
##
################################################################################
if __name__ == '__main__':

  ######
  set_exit_handler( on_exit)

  ######
  # Parse args
  if( len( sys.argv) == 1):
    sys.stdout.write( "Usage: %s <server_addr> <server_port>\n\n" %(sys.argv[0]))
    sys.exit( 1)

  ##
  server_addr = sys.argv[1]
  if( len( sys.argv) >= 3):
    server_port = int( sys.argv[2])

  try:
    udp_echo_server( server_addr, server_port)

  except KeyboardInterrupt:
    sys.stdout.write( "\n[*] Keyboard Interrupt! Exiting Application...\n");
