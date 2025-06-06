##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
  libmvtx_io.la \
  libmvtx.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  MvtxClusterizer.h \
  MvtxHitPruner.h \
  MvtxNoiseMap.h \
  MvtxCombinedRawDataDecoder.h \
  CylinderGeom_Mvtx.h \
  SegmentationAlpide.h \
  MvtxPixelMask.h \
  MvtxPixelMaskv1.h \
  MvtxRawPixelDefs.h \
  MvtxRawPixelMasker.h 


ROOTDICTS = \
  CylinderGeom_Mvtx_Dict.cc \
  MvtxNoiseMap_Dict.cc \
  MvtxPixelMask_Dict.cc \
  MvtxPixelMaskv1_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  CylinderGeom_Mvtx_Dict_rdict.pcm \
	MvtxNoiseMap_Dict_rdict.pcm \
  MvtxPixelMask_Dict_rdict.pcm \
  MvtxPixelMaskv1_Dict_rdict.pcm

# sources for mvtx library
libmvtx_la_SOURCES = \
  MvtxClusterizer.cc \
  MvtxHitPruner.cc \
  MvtxCombinedRawDataDecoder.cc \
  MvtxRawPixelMasker.cc

libmvtx_la_LIBADD = \
  libmvtx_io.la \
  -lCLHEP \
  -lfun4all \
  -lffarawobjects \
  -lfun4allraw \
  -lffamodules \
  -lphg4hit \
  -lSubsysReco \
  -ltrack_io \
  -lmvtx_decoder \
  -ltrackbase_historic_io \
  -lcdbobjects

# sources for io library
libmvtx_io_la_SOURCES = \
  $(ROOTDICTS) \
  CylinderGeom_Mvtx.cc \
  MvtxNoiseMap.cc \
  SegmentationAlpide.cc \
  MvtxPixelMaskv1.cc \
  MvtxRawPixelDefs.cc 

libmvtx_io_la_LIBADD = \
  -lActsCore \
  -lphool \
  -lfun4all \
  -lffamodules \
  -lffarawobjects \
  -lcdbobjects \
  -lCLHEP \
  -lfun4allraw \
  -lphg4hit \
  -ltrack_io \
  -lmvtx_decoder \
  -lg4detectors_io

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;



################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_mvtx_io \
  testexternals_mvtx

testexternals_mvtx_io_SOURCES = testexternals.cc
testexternals_mvtx_io_LDADD = libmvtx_io.la

testexternals_mvtx_SOURCES = testexternals.cc
testexternals_mvtx_LDADD = libmvtx.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
