#
# Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

######## SGX SDK Settings ########

SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= HW
SGX_ARCH ?= x64
SGX_DEBUG ?= 1

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_CFLAGS += -O0 -g
else
        SGX_COMMON_CFLAGS += -O2
endif

######## App Settings ########

ifneq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts_sim
else
	Urts_Library_Name := sgx_urts
endif

App_Cpp_Files := App/App.cpp #$(wildcard App/Edger8rSyntax/*.cpp) $(wildcard App/TrustedLibrary/*.cpp)
App_Include_Paths := -IInclude -IApp -I$(SGX_SDK)/include

App_C_Flags := $(SGX_COMMON_CFLAGS) -fPIC -Wno-attributes $(App_Include_Paths)

# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
        App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
        App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
        App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cpp_Flags := $(App_C_Flags) -std=c++11
App_Link_Flags := $(SGX_COMMON_CFLAGS) -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -lpthread 

ifneq ($(SGX_MODE), HW)
	App_Link_Flags += -lsgx_uae_service_sim
else
	App_Link_Flags += -lsgx_uae_service
endif

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := app

######## Enclave Settings ########

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif
Crypto_Library_Name := sgx_tcrypto

Enclave_1_Cpp_Files := Enclave_1/Enclave.cpp #$(wildcard Enclave/Edger8rSyntax/*.cpp) $(wildcard Enclave/TrustedLibrary/*.cpp)
Enclave_1_Include_Paths := -IInclude -IEnclave_1 -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

Enclave_2_Cpp_Files := Enclave_2/Enclave.cpp #$(wildcard Enclave/Edger8rSyntax/*.cpp) $(wildcard Enclave/TrustedLibrary/*.cpp)
Enclave_2_Include_Paths := -IInclude -IEnclave_2 -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

Enclave_3_Cpp_Files := Enclave_3/Enclave.cpp #$(wildcard Enclave/Edger8rSyntax/*.cpp) $(wildcard Enclave/TrustedLibrary/*.cpp)
Enclave_3_Include_Paths := -IInclude -IEnclave_3 -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

Outer_Enclave_Cpp_Files := Semi/Semi.cpp #$(wildcard Semi/Edger8rSyntax/*.cpp) $(wildcard Semi/TrustedLibrary/*.cpp)
Outer_Enclave_Include_Paths := -IInclude -ISemi -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
	Enclave_1_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector
	Enclave_2_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector
	Enclave_3_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector
else
	Enclave_1_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector-strong
	Enclave_2_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector-strong
	Enclave_3_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections -fstack-protector-strong
endif

Enclave_1_C_Flags += $(Enclave_1_Include_Paths)
Enclave_1_Cpp_Flags := $(Enclave_1_C_Flags) -std=c++11 -nostdinc++

Enclave_2_C_Flags += $(Enclave_2_Include_Paths)
Enclave_2_Cpp_Flags := $(Enclave_2_C_Flags) -std=c++11 -nostdinc++

Enclave_3_C_Flags += $(Enclave_3_Include_Paths)
Enclave_3_Cpp_Flags := $(Enclave_3_C_Flags) -std=c++11 -nostdinc++

Outer_Enclave_C_Flags += $(Outer_Enclave_Include_Paths)
Outer_Enclave_Cpp_Flags := $(Outer_Enclave_C_Flags) -std=c++11 -nostdinc++

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
#	-Wl,--whole-archive -lsgx_tcmalloc -Wl,--no-whole-archive 
Enclave_1_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave_1/Enclave.lds

Enclave_2_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave_2/Enclave.lds

Enclave_3_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave_3/Enclave.lds

#	-Wl,--whole-archive -lsgx_tcmalloc -Wl,--no-whole-archive 
Outer_Enclave_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Semi/Enclave.lds

Enclave_1_Cpp_Objects := $(Enclave_1_Cpp_Files:.cpp=.o)
Enclave_2_Cpp_Objects := $(Enclave_2_Cpp_Files:.cpp=.o)
Enclave_3_Cpp_Objects := $(Enclave_3_Cpp_Files:.cpp=.o)

Outer_Enclave_Cpp_Objects := $(Outer_Enclave_Cpp_Files:.cpp=.o)

Enclave_1_Name := enclave_1.so
Signed_Enclave_1_Name := enclave_1.signed.so
Enclave_1_Config_File := Enclave_1/Enclave.config.xml

Enclave_2_Name := enclave_2.so
Signed_Enclave_2_Name := enclave_2.signed.so
Enclave_2_Config_File := Enclave_2/Enclave.config.xml

Enclave_3_Name := enclave_3.so
Signed_Enclave_3_Name := enclave_3.signed.so
Enclave_3_Config_File := Enclave_3/Enclave.config.xml

Outer_Enclave_Name := enclave_outer.so
Signed_Outer_Enclave_Name := enclave_outer.signed.so
Outer_Enclave_Config_File := Semi/Enclave.config.xml

ifeq ($(SGX_MODE), HW)
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = HW_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = HW_PRERELEASE
else
	Build_Mode = HW_RELEASE
endif
else
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = SIM_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = SIM_PRERELEASE
else
	Build_Mode = SIM_RELEASE
endif
endif


.PHONY: all run

ifeq ($(Build_Mode), HW_RELEASE)
all: .config_$(Build_Mode)_$(SGX_ARCH) $(App_Name) $(Enclave_1_Name) $(Enclave_2_Name) $(Enclave_3_Name) $(Outer_Enclave_Name)
	@echo "The project has been built in release hardware mode."
	@echo "Please sign the $(Enclave_Name) first with your signing key before you run the $(App_Name) to launch and access the enclave."
	@echo "To sign the enclave use the command:"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <your key> -enclave $(Enclave_Name) -out <$(Signed_Enclave_Name)> -config $(Enclave_Config_File)"
	@echo "You can also sign the enclave using an external signing tool."
	@echo "To build the project in simulation mode set SGX_MODE=SIM. To build the project in prerelease mode set SGX_PRERELEASE=1 and SGX_MODE=HW."
else
all: .config_$(Build_Mode)_$(SGX_ARCH) $(App_Name) $(Signed_Enclave_1_Name) $(Signed_Enclave_2_Name) $(Signed_Enclave_3_Name) $(Signed_Outer_Enclave_Name)
ifeq ($(Build_Mode), HW_DEBUG)
	@echo "The project has been built in debug hardware mode."
else ifeq ($(Build_Mode), SIM_DEBUG)
	@echo "The project has been built in debug simulation mode."
else ifeq ($(Build_Mode), HW_PRERELEASE)
	@echo "The project has been built in pre-release hardware mode."
else ifeq ($(Build_Mode), SIM_PRERELEASE)
	@echo "The project has been built in pre-release simulation mode."
else
	@echo "The project has been built in release simulation mode."
endif
endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/$(App_Name)
	@echo "RUN  =>  $(App_Name) [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif

######## App Objects ########

App/Enclave_1_u.c: $(SGX_EDGER8R) Enclave_1/Enclave_1.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave_1/Enclave_1.edl --search-path ../Enclave_1 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Enclave_2_u.c: $(SGX_EDGER8R) Enclave_2/Enclave_2.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave_2/Enclave_2.edl --search-path ../Enclave_2 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Enclave_3_u.c: $(SGX_EDGER8R) Enclave_3/Enclave_3.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave_3/Enclave_3.edl --search-path ../Enclave_3 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Semi_u.c: $(SGX_EDGER8R) Semi/Semi.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Semi/Semi.edl --search-path ../Semi --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Enclave_1_u.o: App/Enclave_1_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/Enclave_2_u.o: App/Enclave_2_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/Enclave_3_u.o: App/Enclave_3_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/Semi_u.o: App/Semi_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp
	@$(CXX) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/Enclave_1_u.o App/Enclave_2_u.o App/Enclave_3_u.o App/Semi_u.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"

.config_$(Build_Mode)_$(SGX_ARCH):
	@rm -f .config_* $(App_Name) $(Enclave_1_Name) $(Enclave_2_Name) $(Enclave_3_Name) $(Signed_Enclave_1_Name) $(Signed_Enclave_2_Name) $(Signed_Enclave_3_Name) $(Signed_Outer_Enclave_Name) $(App_Cpp_Objects) App/Enclave_1_u.* App/Enclave_2_u.* App/Enclave_3_u.* $(Enclave_1_Cpp_Objects) $(Enclave_2_Cpp_Objects) $(Enclave_3_Cpp_Objects) Enclave_1/Enclave_1_t.* Enclave_2/Enclave_2_t.* Enclave_3/Enclave_3_t.* App/Semi_u.* $(Outer_Enclave_Cpp_Objecte) Semi/Semi_t.*
	@touch .config_$(Build_Mode)_$(SGX_ARCH)

######## Outer Enclave Objects ########

Semi/DEnclave_1_u.c: $(SGX_EDGER8R) Enclave_1/DEnclave_1.edl
	@cd Semi && $(SGX_EDGER8R) --untrusted ../Enclave_1/DEnclave_1.edl --search-path ../Semi --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Semi/DEnclave_2_u.c: $(SGX_EDGER8R) Enclave_2/DEnclave_2.edl
	@cd Semi && $(SGX_EDGER8R) --untrusted ../Enclave_2/DEnclave_2.edl --search-path ../Semi --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Semi/DEnclave_3_u.c: $(SGX_EDGER8R) Enclave_3/DEnclave_3.edl
	@cd Semi && $(SGX_EDGER8R) --untrusted ../Enclave_3/DEnclave_3.edl --search-path ../Semi --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Semi/DEnclave_1_u.o: Semi/DEnclave_1_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@ 
	@echo "CC   <=  $<"

Semi/DEnclave_2_u.o: Semi/DEnclave_2_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@ 
	@echo "CC   <=  $<"

Semi/DEnclave_3_u.o: Semi/DEnclave_3_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@ 
	@echo "CC   <=  $<"

Semi/Semi_t.c: $(SGX_EDGER8R) Semi/Semi.edl
	@cd Semi && $(SGX_EDGER8R) --trusted ../Semi/Semi.edl --search-path ../Semi --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Semi/Semi_t.o: Semi/Semi_t.c
	@$(CC) $(Outer_Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Semi/%.o: Semi/%.cpp
	@$(CXX) $(Outer_Enclave_Cpp_Flags) -fPIC -c $< -o $@
	@echo "CXX  <=  $<"

$(Outer_Enclave_Name): Semi/Semi_t.o Semi/DEnclave_1_u.o Semi/DEnclave_2_u.o Semi/DEnclave_3_u.o $(Outer_Enclave_Cpp_Objects)
	@$(CXX) $^ -o $@ $(Outer_Enclave_Link_Flags) #-lsgx_urts_sim  -lsgx_uae_service_sim
	@echo "LINK =>  $@"

$(Signed_Outer_Enclave_Name): $(Outer_Enclave_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Semi/Enclave_private.pem -enclave $(Outer_Enclave_Name) -out $@ -config $(Outer_Enclave_Config_File)
	@echo "SIGN =>  $@"

######## Enclave Objects ########

######## Enclave 1 ########
Enclave_1/Enclave_1_t.c: $(SGX_EDGER8R) Enclave_1/Enclave_1.edl
	@cd Enclave_1 && $(SGX_EDGER8R) --nested --trusted ../Enclave_1/Enclave_1.edl --search-path ../Enclave_1 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_1/DEnclave_1_t.c: $(SGX_EDGER8R) Enclave_1/DEnclave_1.edl
	@cd Enclave_1 && $(SGX_EDGER8R) --demi --trusted ../Enclave_1/DEnclave_1.edl --search-path ../Enclave_1 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_1/Enclave_1_t.o: Enclave_1/Enclave_1_t.c
	@$(CC) $(Enclave_1_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_1/DEnclave_1_t.o: Enclave_1/DEnclave_1_t.c
	@$(CC) $(Enclave_1_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_1/%.o: Enclave_1/%.cpp
	@$(CXX) $(Enclave_1_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(Enclave_1_Name): Enclave_1/DEnclave_1_t.o Enclave_1/Enclave_1_t.o $(Enclave_1_Cpp_Objects)
	@$(CXX) $^ -o $@ $(Enclave_1_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_1_Name): $(Enclave_1_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave_1/Enclave_private.pem -enclave $(Enclave_1_Name) -out $@ -config $(Enclave_1_Config_File)
	@echo "SIGN =>  $@"

######## Enclave 2 ########
Enclave_2/Enclave_2_t.c: $(SGX_EDGER8R) Enclave_2/Enclave_2.edl
	@cd Enclave_2 && $(SGX_EDGER8R) --nested --trusted ../Enclave_2/Enclave_2.edl --search-path ../Enclave_2 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_2/DEnclave_2_t.c: $(SGX_EDGER8R) Enclave_2/DEnclave_2.edl
	@cd Enclave_2 && $(SGX_EDGER8R) --demi --trusted ../Enclave_2/DEnclave_2.edl --search-path ../Enclave_2 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_2/Enclave_2_t.o: Enclave_2/Enclave_2_t.c
	@$(CC) $(Enclave_2_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_2/DEnclave_2_t.o: Enclave_2/DEnclave_2_t.c
	@$(CC) $(Enclave_2_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_2/%.o: Enclave_2/%.cpp
	@$(CXX) $(Enclave_2_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(Enclave_2_Name): Enclave_2/DEnclave_2_t.o Enclave_2/Enclave_2_t.o $(Enclave_2_Cpp_Objects)
	@$(CXX) $^ -o $@ $(Enclave_2_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_2_Name): $(Enclave_2_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave_2/Enclave_private.pem -enclave $(Enclave_2_Name) -out $@ -config $(Enclave_2_Config_File)
	@echo "SIGN =>  $@"

######## Enclave 3 ########
Enclave_3/Enclave_3_t.c: $(SGX_EDGER8R) Enclave_3/Enclave_3.edl
	@cd Enclave_3 && $(SGX_EDGER8R) --nested --trusted ../Enclave_3/Enclave_3.edl --search-path ../Enclave_3 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_3/DEnclave_3_t.c: $(SGX_EDGER8R) Enclave_3/DEnclave_3.edl
	@cd Enclave_3 && $(SGX_EDGER8R) --demi --trusted ../Enclave_3/DEnclave_3.edl --search-path ../Enclave_3 --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave_3/Enclave_t.o: Enclave_3/Enclave_3_t.c
	@$(CC) $(Enclave_3_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_3/DEnclave_3_t.o: Enclave_3/DEnclave_3_t.c
	@$(CC) $(Enclave_3_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_3/%.o: Enclave_3/%.cpp
	@$(CXX) $(Enclave_3_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(Enclave_3_Name): Enclave_3/DEnclave_3_t.o Enclave_3/Enclave_t.o $(Enclave_3_Cpp_Objects)
	@$(CXX) $^ -o $@ $(Enclave_3_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_3_Name): $(Enclave_3_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave_3/Enclave_private.pem -enclave $(Enclave_3_Name) -out $@ -config $(Enclave_3_Config_File)
	@echo "SIGN =>  $@"

.PHONY: clean

clean:
	@rm -f .config_* $(App_Name) $(Enclave_1_Name) $(Enclave_2_Name) $(Enclave_3_Name) $(Outer_Enclave_Name) $(Signed_Enclave_1_Name) $(Signed_Enclave_2_Name) $(Signed_Enclave_3_Name) $(Signed_Outer_Enclave_Name) $(App_Cpp_Objects) $(Enclave_1_Cpp_Objects) $(Enclave_2_Cpp_Objects) $(Enclave_3_Cpp_Objects) $(Outer_Enclave_Cpp_Objects) App/Enclave_1_u.* App/Enclave_2_u.* App/Enclave_3_u.* Enclave_1/DEnclave_1_t.* Enclave_2/DEnclave_2_t.* Enclave_3/DEnclave_3_t.* Enclave_1/Enclave_1_t.* Enclave_2/Enclave_2_t.* Enclave_3/Enclave_3_t.* App/Semi_u.* Semi/Semi_t.*  
