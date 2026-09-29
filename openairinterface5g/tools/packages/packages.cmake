# SPDX-License-Identifier: LicenseRef-CSSL-1.0

set(COMPONENTS_LIST "")

set(CPACK_PACKAGE_VERSION ${OAI_VERSION})
set(CPACK_PACKAGE_VENDOR "OpenAirInterface")
set(CPACK_PACKAGE_CONTACT "OAI Maintainers <oaicicdteam@openairinterface.org>") #Even though it does not appear on CPACK global documention it works, it appears here: https://cmake.org/cmake/help/latest/cpack_gen/deb.html#variable:CPACK_DEBIAN_PACKAGE_MAINTAINER
set(CPACK_MONOLITHIC_INSTALL OFF)
set(CPACK_PACKAGING_INSTALL_PREFIX "/")

if(PACKAGING_RPM)
  set(CPACK_GENERATOR "RPM")
  set(CPACK_RPM_DEFAULT_DIR_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )
  set(CPACK_RPM_COMPONENT_INSTALL ON)
  set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  set(CPACK_RPM_PACKAGE_URL "https://openairinterface.org")
  set(CPACK_RPM_PACKAGE_LICENSE "Collaborative Standards Software License v1.0")
  set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
  set(SYSTEMD_UNIT_DIR "/usr/lib/systemd/system")
  set(CPACK_RPM_DEBUGINFO_PACKAGE ON)
  set(CPACK_BUILD_SOURCE_DIRS "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/radio/USRP")
  set(CPACK_RPM_BUILD_SOURCE_DIRS_PREFIX .)
else()
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_DEBIAN_PACKAGE_SECTION "Networking")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
  elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
  elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
  else()
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")  # fallback
  endif()
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://openairinterface.org")
  set(SYSTEMD_UNIT_DIR "/lib/systemd/system")
  set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
endif()


##### LTE
if(PACKAGING_LTE)
  list(APPEND COMPONENTS_LIST oai-lte)
  set(LTE_PACKAGE_DESCRIPTION "OAI is an open-source software dedicated to LTE and 5G NR networks, including both Radio Access Network and Core Network.
    Allowing simulation of mobile networks, prototyping, testing and providing an environment for research. OAI supports real-time operations with hardware radios such as USRP and
    software-only simulations using virtual radio interfaces. It includes modules for PHY, MAC, RLC, PDCP, RRC, SDAP, and core network functions, and offer tools for I/Q recording
    and playback, channel coding tests, and performance evaluation of uplink and downlink channels. OAI enables multi-UE and multi-gNB deployments, supports centralized/distributed
    5G RAN architectures (CU/DU , (n)FAPI, and 7.2 splits) and provides software to setup testbeds for experimental purposes.
    This package contains the RAN part of LTE built by OAI, including lte-softmodem a general full-stack modem, lte-uesoftmodem a UE-only softmodem, conf2uedata a converter tool
    for configurations files to produce UE-specific binary data, nvram to stores permanently the UE or network parameters and finally usim to stores subscriber identity (IMSI),
    authentication keys and operator credentials.")

  if(PACKAGING_RPM)
    set(CPACK_RPM_OAI-LTE_PACKAGE_REQUIRES "liboai-common")
    set(CPACK_RPM_OAI-LTE_PACKAGE_RELOCATABLE FALSE)
    set(CPACK_RPM_PACKAGE_OAI-LTE_SUMMARY "OpenAirInterface LTE package")
    set(CPACK_RPM_PACKAGE_OAI-LTE_DESCRIPTION "${LTE_PACKAGE_DESCRIPTION}")
  else()
    set(CPACK_DEBIAN_OAI-LTE_PACKAGE_DEPENDS "liboai-common")
    set(CPACK_DEBIAN_OAI-LTE_SUMMARY "OpenAirInterface LTE package")
    set(CPACK_DEBIAN_OAI-LTE_DESCRIPTION "${LTE_PACKAGE_DESCRIPTION}")
  endif()

  # Retrieve all LTE related configuration files inside targets and ci-scripts folders
  set(CONFIG_FILELIST_LTE "")
  set(CONFIG_FILENAME_LIST_LTE "")
  file(GLOB LTE_GENERIC_LIBS "${CMAKE_SOURCE_DIR}/targets/PROJECTS/GENERIC-LTE-EPC/CONF/*.conf")
  file(GLOB LTE_LIBS "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/*lte*.conf")
  foreach(file ${LTE_LIBS} ${LTE_GENERIC_LIBS})
    get_filename_component(fname ${file} NAME)
    list(APPEND CONFIG_FILENAME_LIST_LTE ${fname})
    install(FILES "${file}" DESTINATION etc/openairinterface/lte COMPONENT oai-lte)
  endforeach()
  foreach(config ${CONFIG_FILENAME_LIST_LTE})
    if(PACKAGING_RPM)
      list(APPEND CONFIG_FILELIST_LTE "%config(noreplace) /etc/openairinterface/lte/${config}")
    else()
      list(APPEND CONFIG_FILELIST_LTE "/etc/openairinterface/lte/${config}")
    endif()
  endforeach()

  #Add package control files to the package (postinst, prerm, conffiles)
  if(PACKAGING_RPM)
    set(CPACK_RPM_OAI-LTE_USER_FILELIST "${CONFIG_FILELIST_LTE}")
    set(CPACK_RPM_OAI-LTE_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-lte/postinst")
    set(CPACK_RPM_OAI-LTE_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-lte/prerm")
  else()
    set(CONFFILES_PATH_LTE "${CMAKE_SOURCE_DIR}/tools/packages/oai-lte/conffiles")
    file(WRITE "${CONFFILES_PATH_LTE}" "")
    foreach(cfg IN LISTS CONFIG_FILELIST_LTE)
      file(APPEND "${CONFFILES_PATH_LTE}" "${cfg}\n")
    endforeach()
    file(CHMOD "${CMAKE_SOURCE_DIR}/tools/packages/oai-lte/conffiles" PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    set(CPACK_DEBIAN_OAI-LTE_PACKAGE_CONTROL_EXTRA
        "${CONFFILES_PATH_LTE}"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-lte/postinst"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-lte/prerm"
        )
  endif()

  #Add the systemd services to the package
  install(
    FILES ${CMAKE_SOURCE_DIR}/tools/packages/systemd_services/oai-lte/lte-softmodem.service
    DESTINATION ${SYSTEMD_UNIT_DIR}
    COMPONENT oai-lte
  )

  #Add the targets to the package
  install(TARGETS lte-softmodem lte-uesoftmodem conf2uedata usim nvram
    COMPONENT oai-lte
    RUNTIME DESTINATION /usr/bin
    LIBRARY DESTINATION /usr/lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
endif()

##### NR

if(PACKAGING_NR)
  list(APPEND COMPONENTS_LIST oai-nr)
  set(NR_PACKAGE_DESCRIPTION "OAI is an open-source software dedicated to LTE and 5G NR networks, including both Radio Access Network and Core Network.
    Allowing simulation of mobile networks, prototyping, testing and providing an environment for research. OAI supports real-time operations with hardware radios such as USRP and
    software-only simulations using virtual radio interfaces. It includes modules for PHY, MAC, RLC, PDCP, RRC, SDAP, and core network functions, and offer tools for I/Q recording
    and playback, channel coding tests, and performance evaluation of uplink and downlink channels. OAI enables multi-UE and multi-gNB deployments, supports centralized/distributed
    5G RAN architectures (CU/DU , (n)FAPI, and 7.2 splits) and provides software to setup testbeds for experimental purposes.
    This package contains the RAN part of 5G NR built by OAI, including nr-softmodem a general full-stack modem, nr-uesoftmodem a UE-only softmodem and nr-cuup a split 5G gNB
    for distributed deployments.")

  if(PACKAGING_RPM)
    set(CPACK_RPM_OAI-NR_PACKAGE_REQUIRES "liboai-common")
    set(CPACK_RPM_OAI-NR_PACKAGE_RELOCATABLE FALSE)
    set(CPACK_RPM_PACKAGE_OAI-NR_SUMMARY "OpenAirInterface NR package")
    set(CPACK_RPM_PACKAGE_OAI-NR_DESCRIPTION "${NR_PACKAGE_DESCRIPTION}")
  else()
    set(CPACK_DEBIAN_OAI-NR_PACKAGE_DEPENDS "liboai-common")
    set(CPACK_DEBIAN_OAI-NR_SUMMARY "OpenAirInterface NR package")
    set(CPACK_DEBIAN_OAI-NR_DESCRIPTION "${NR_PACKAGE_DESCRIPTION}")
  endif()


  # Retrieve all NR related configuration files inside targets and ci-scripts folders
  set(CONFIG_FILELIST_NR "")
  set(CONFIG_FILENAME_LIST_NR "")
  file(GLOB NR_CI_LIBS "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/*nr*.conf")
  file(GLOB NR_LIBS "${CMAKE_SOURCE_DIR}/targets/PROJECTS/GENERIC-NR-5GC/CONF/*.conf")
  file(GLOB CUUP_LIBS "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/*cuup*.conf")
  foreach(file ${NR_LIBS} ${NR_CI_LIBS} ${CUUP_LIBS})
    get_filename_component(fname ${file} NAME)
    list(APPEND CONFIG_FILENAME_LIST_NR ${fname})
    install(FILES "${file}" DESTINATION etc/openairinterface/nr COMPONENT oai-nr)
  endforeach()
  install(FILES "${CMAKE_SOURCE_DIR}/doc/tutorial_resources/oai-cn5g/conf/config.yaml" DESTINATION etc/openairinterface/nr COMPONENT oai-nr)
  foreach(config ${CONFIG_FILENAME_LIST_NR})
    if(PACKAGING_RPM)
      list(APPEND CONFIG_FILELIST_NR "%config(noreplace) /etc/openairinterface/nr/${config}")
    else()
      list(APPEND CONFIG_FILELIST_NR "/etc/openairinterface/nr/${config}")
    endif()
  endforeach()

  if(PACKAGING_RPM)
    list(APPEND CONFIG_FILELIST_NR "%config(noreplace) /etc/openairinterface/nr/config.yaml")
  else()
    list(APPEND CONFIG_FILELIST_NR "/etc/openairinterface/nr/config.yaml")
  endif()

  #Add package control files to the package (postinst, prerm, conffiles)
  if(PACKAGING_RPM)
    set(CPACK_RPM_OAI-NR_USER_FILELIST "${CONFIG_FILELIST_NR}")
    set(CPACK_RPM_OAI-NR_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-nr/postinst")
    set(CPACK_RPM_OAI-NR_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-nr/prerm")
  else()
    set(CONFFILES_PATH_NR "${CMAKE_SOURCE_DIR}/tools/packages/oai-nr/conffiles")
    file(WRITE "${CONFFILES_PATH_NR}" "")
    foreach(cfg IN LISTS CONFIG_FILELIST_NR)
      file(APPEND "${CONFFILES_PATH_NR}" "${cfg}\n")
    endforeach()
    file(CHMOD "${CMAKE_SOURCE_DIR}/tools/packages/oai-nr/conffiles" PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    set(CPACK_DEBIAN_OAI-NR_PACKAGE_CONTROL_EXTRA
        "${CONFFILES_PATH_NR}"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-nr/postinst"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/oai-nr/prerm"
        )
  endif()

  #Add documentation to the package
  foreach(document NR_SA_Tutorial_COTS_UE NR_SA_Tutorial_OAI_CN5G NR_SA_Tutorial_OAI_multi_UE NR_SA_Tutorial_OAI_nrUE)
    install(FILES "${CMAKE_SOURCE_DIR}/doc/${document}.md" DESTINATION share/doc/oai-nr COMPONENT oai-nr)
  endforeach()

  #Add the systemd services to the package
  install(
    FILES ${CMAKE_SOURCE_DIR}/tools/packages/systemd_services/oai-nr/nr-cuup.service ${CMAKE_SOURCE_DIR}/tools/packages/systemd_services/oai-nr/nr-softmodem.service
    DESTINATION ${SYSTEMD_UNIT_DIR}
    COMPONENT oai-nr
  )


  #Add the targets to the package
  install(TARGETS nr-softmodem nr-cuup nr-uesoftmodem
    COMPONENT oai-nr
    RUNTIME DESTINATION /usr/bin
    LIBRARY DESTINATION /usr/lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
endif()

##### PHYSIM

if(PACKAGING_PHYSIM)
  list(APPEND COMPONENTS_LIST oai-physim)
  # NOTE: Following simulators have to be fixed before added -> pbchsim mbmssim pdcchsim pucchsim prachsim syncsim scansim
  set(PHYSIM_PACKAGE_DESCRIPTION "This package contains several simulation programs allowing developers to validate, debug and benchmark specific algorithm and PHY channels in isolation
    --------------------------------------------------------
    Coding focused: polartest, smallblocktest, ldpctest
    Downlink PHY: nr_dlschsim, nr_psbchsim, nr_dlsim
    Uplink PHY: nr_pucchsim nr_prachsim nr_ulschsim nr_ulsim nr_ulsim_mu_mimo
    --------------------------------------------------------
    polartest: test polar codes which are used in 5G control channels, validates encoding, decoding and error correction performance*
    smallblocktest: generic test for small channel coding blocks, validates encoding and decoding for small transport blocks
    ldpctest: test low density parity check, validates encoding, decoding, throughput and bit error rates (BER)
    nr_dlschsim: simulates the NR Downlink Shared Channel, including encoding, decoding, modulation, precoding, transmission
    nr_psbchsim: simulates the Physical Broadcast Channel, including encoding, decoding, DMRS mapping and CRC checks
    nr_pucchsim: simulates the Physical Uplink Control Channel, including uplink control signaling
    nr_dlsim: simulates NR downlink, including multiple downlink channels and measurements of BLER/throughput
    nr_prachsim: simulates the Physical Random Access Channel, including UE initial access, preamble detection and timing alignment
    nr_ulschsim: simulates the Uplink Shared Channel, including LDPC encoding, modulation, transmission and decoding
    nr_ulsim: simulates the NR uplink simulation, including PUCCH, PUSCH and PRACH
    nr_ulsim_mu_mimo: simulates the NR uplink simulation with PUSCH multi user MIMO")

  if(PACKAGING_RPM)
    set(CPACK_RPM_PACKAGE_OAI-PHYSIM_SUMMARY "OpenAirInterface PhySim package")
    set(CPACK_RPM_PACKAGE_OAI-PHYSIM_DESCRIPTION "${PHYSIM_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_OAI-PHYSIM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst")
    set(CPACK_RPM_OAI-PHYSIM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm")
  else()
    set(CPACK_DEBIAN_OAI-PHYSIM_SUMMARY "OpenAirInterface PhySim package")
    set(CPACK_DEBIAN_OAI-PHYSIM_DESCRIPTION "${PHYSIM_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_OAI-PHYSIM_PACKAGE_CONTROL_EXTRA
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm"
      )
  endif()


  #Add documentation to the package
  install(FILES "${CMAKE_SOURCE_DIR}/doc/physical-simulators.md" DESTINATION share/doc/ COMPONENT oai-physim)

  #Add the targets to the package
  install(TARGETS polartest smallblocktest ldpctest nr_dlschsim nr_psbchsim nr_pucchsim nr_dlsim nr_prachsim nr_ulschsim nr_ulsim nr_ulsim_mu_mimo
        COMPONENT oai-physim
        RUNTIME DESTINATION /usr/bin
        LIBRARY DESTINATION /usr/lib
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      )
endif()

##### USRP

if(PACKAGING_USRP)
  list(APPEND COMPONENTS_LIST liboai-usrp)
  set(USRP_PACKAGE_DESCRIPTION "Universal Software Radio Peripheral (USRP) are radio equipments used for SDR (software-defined radio) applications.
    They are used to transmit and receive radio signals, to do the conversions between analog RF signals and digital baseband and to handle real time signal processing tasks.
    To use USRP, OAI depends on libuhd a shared library provided by Ettus Research, it allows the detection and connection to the device, configure the RF parameters and many
    other features. OAI uses libuhd to send and receive I/Q samples with the USRP in real time.
    oai_usrpdevif is the library used by OAI to connect the softmodem to the USRP, it provides an abstraction between phy layer of OAI and the USRP.")

  if(PACKAGING_RPM)
    set(CPACK_RPM_LIBOAI-USRP_PACKAGE_REQUIRES "liboai-common")
    set(CPACK_RPM_LIBOAI-USRP_PACKAGE_PROVIDES "liboai_usrpdevif.so()(64bit)")
    set(CPACK_RPM_LIBOAI-USRP_PACKAGE_RELOCATABLE FALSE)
    set(CPACK_RPM_PACKAGE_LIBOAI-USRP_SUMMARY "OpenAirInterface USRP package")
    set(CPACK_RPM_PACKAGE_LIBOAI-USRP_DESCRIPTION "${USRP_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_LIBOAI-USRP_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst")
    set(CPACK_RPM_LIBOAI-USRP_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm")
  else()
    set(CPACK_DEBIAN_LIBOAI-USRP_PACKAGE_DEPENDS "liboai-common, libc6")
    set(CPACK_DEBIAN_LIBOAI-USRP_SUMMARY "OpenAirInterface USRP package")
    set(CPACK_DEBIAN_LIBOAI-USRP_DESCRIPTION "${USRP_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_LIBOAI-USRP_PACKAGE_CONTROL_EXTRA
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm"
      )
  endif()

  #Add documentation to the package
  install(FILES "${CMAKE_SOURCE_DIR}/radio/USRP/README.md" DESTINATION share/doc/liboai-usrp COMPONENT liboai-usrp)

  # Retrieve all USRP related configuration files inside ci-scripts folder
  set(CONFIG_FILELIST_USRP "")
  set(CONFIG_FILENAME_LIST_USRP "")
  file(GLOB USRP_LIBS "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/*usrp*.conf")
  foreach(file ${USRP_LIBS})
    get_filename_component(fname ${file} NAME)
    list(APPEND CONFIG_FILENAME_LIST_USRP ${fname})
  endforeach()
  foreach(config ${CONFIG_FILENAME_LIST_USRP})
    install(FILES "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/${config}" DESTINATION etc/openairinterface/usrp COMPONENT liboai-usrp)
    if(PACKAGING_RPM)
      list(APPEND CONFIG_FILELIST_USRP "%config(noreplace) /etc/openairinterface/usrp/${config}")
    else()
      list(APPEND CONFIG_FILELIST_USRP "/etc/openairinterface/usrp/${config}")
    endif()
  endforeach()

  #Add package control files to the package (postinst, prerm, conffiles)
  if(PACKAGING_RPM)
    set(CPACK_RPM_LIBOAI-USRP_USER_FILELIST "${CONFIG_FILELIST_USRP}")
  else()
    set(CONFFILES_PATH_USRP "${CMAKE_SOURCE_DIR}/tools/packages/liboai-usrp/conffiles")
    file(WRITE "${CONFFILES_PATH_USRP}" "")
    foreach(cfg IN LISTS CONFIG_FILELIST_USRP)
      file(APPEND "${CONFFILES_PATH_USRP}" "${cfg}\n")
    endforeach()
    file(CHMOD "${CMAKE_SOURCE_DIR}/tools/packages/liboai-usrp/conffiles" PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    set(CPACK_DEBIAN_LIBOAI-USRP_PACKAGE_CONTROL_EXTRA "${CONFFILES_PATH_USRP};${CMAKE_SOURCE_DIR}/tools/packages/triggers;")
  endif()


  #Add the targets to the package
  install(TARGETS oai_usrpdevif
          COMPONENT liboai-usrp
          RUNTIME DESTINATION /usr/bin
          LIBRARY DESTINATION /usr/lib
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
        )
endif()

##### COMMON

if(PACKAGING_COMMON)
  list(APPEND COMPONENTS_LIST liboai-common)
  set(COMMON_PACKAGE_DESCRIPTION "This package contains shared libraries for the other OAI packages
    params_libconfig: helper library for configuration management, it abstracts the parsing of .conf files and organizes parameters in a structured way
    dfts: provides Discrete Fourier Transformations functions
    oai_iqplayer: a tool which allows any softmodem to capture subframes from a USRP into a file and later replay them, enabling offline testing, debugging and reproducible experiments without live RF transmission
    params_yaml: YAML-based configuration sustem for defining and loading runtime parameters in a structured and readable way
    telnetsrv: built-in server that lets you remotely monitor and interact with a running softmodem for debugging and testing
    coding: handles all physical-layer channel coding, decoding, and error-checking operations
    rfsimulator: emulates radio front-end to enables simulation of boardless RF transmissions and receptions
    ldpc and ldpc_orig: two versions of LDPC implementation, first one is optimized but second one is simpler and easier to read
    vrtsim: allows end-to-end PHY/MAC testing, does not requires real hardware")
  if(PACKAGING_RPM)
    set(CPACK_RPM_LIBOAI-COMMON_PACKAGE_REQUIRES "libconfig-devel >= 1.4.9")
    set(CPACK_RPM_LIBOAI-COMMON_PACKAGE_PROVIDES "libcoding.so()(64bit), libdfts.so()(64bit), libldpc.so()(64bit), libldpc_orig.so()(64bit), liboai_iqplayer.so()(64bit), libparams_libconfig.so()(64bit), libparams_yaml.so()(64bit), librfsimulator.so()(64bit), libtelnetsrv.so()(64bit), libvrtsim.so()(64bit)")
    set(CPACK_RPM_LIBOAI-COMMON_PACKAGE_RELOCATABLE FALSE)
    set(CPACK_RPM_PACKAGE_LIBOAI-COMMON_SUMMARY "OpenAirInterface shared libraries package")
    set(CPACK_RPM_PACKAGE_LIBOAI-COMMON_DESCRIPTION "${COMMON_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_LIBOAI-COMMON_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst")
    set(CPACK_RPM_LIBOAI-COMMON_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm")
  else()
    set(CPACK_DEBIAN_LIBOAI-COMMON_PACKAGE_DEPENDS "libc6, libconfig-dev (>=1.4.9), libsctp1")
    set(CPACK_DEBIAN_LIBOAI-COMMON_SUMMARY "OpenAirInterface shared libraries package")
    set(CPACK_DEBIAN_LIBOAI-COMMON_DESCRIPTION "${COMMON_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_LIBOAI-COMMON_PACKAGE_CONTROL_EXTRA
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/postinst"
        "${CMAKE_SOURCE_DIR}/tools/packages/systemd_scripts/prerm"
      )
  endif()

  # Retrieve every rfsim related configuration files inside ci-scripts folder
  set(CONFIG_FILELIST_COMMON "")
  set(CONFIG_FILENAME_LIST_COMMON "")
  file(GLOB COMMON_LIBS_PATH "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/*rfsim*.conf")
  foreach(file ${COMMON_LIBS_PATH})
    get_filename_component(fname ${file} NAME)
    list(APPEND CONFIG_FILENAME_LIST_COMMON ${fname})
  endforeach()

  foreach(config ${CONFIG_FILENAME_LIST_COMMON})
    install(FILES "${CMAKE_SOURCE_DIR}/ci-scripts/conf_files/${config}" DESTINATION etc/openairinterface/liboai-common COMPONENT liboai-common)
    if(PACKAGING_RPM)
      list(APPEND CONFIG_FILELIST_COMMON "%config(noreplace) /etc/openairinterface/liboai-common/${config}")
    else()
      list(APPEND CONFIG_FILELIST_COMMON "/etc/openairinterface/liboai-common/${config}")
    endif()
  endforeach()

  #Add package control files to the package (postinst, prerm, conffiles)
  if(PACKAGING_RPM)
    set(CPACK_RPM_LIBOAI-COMMON_USER_FILELIST "${CONFIG_FILELIST_COMMON}")
  else()
    set(CONFFILES_PATH_COMMON "${CMAKE_SOURCE_DIR}/tools/packages/liboai-common/conffiles")
    file(WRITE "${CONFFILES_PATH_COMMON}" "")
    foreach(cfg IN LISTS CONFIG_FILELIST_COMMON)
      file(APPEND "${CONFFILES_PATH_COMMON}" "${cfg}\n")
    endforeach()
    file(CHMOD "${CMAKE_SOURCE_DIR}/tools/packages/liboai-common/conffiles" PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    set(CPACK_DEBIAN_LIBOAI-COMMON_PACKAGE_CONTROL_EXTRA "${CONFFILES_PATH_COMMON};${CMAKE_SOURCE_DIR}/tools/packages/triggers;")
  endif()


  #Add the targets to the package
  install(TARGETS params_libconfig dfts oai_iqplayer params_yaml telnetsrv coding rfsimulator ldpc_orig ldpc vrtsim
          COMPONENT liboai-common
          RUNTIME DESTINATION /usr/bin
          LIBRARY DESTINATION /usr/lib
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
          )
endif()

#Add the license to every package
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
foreach(component IN LISTS COMPONENTS_LIST)
  install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
          DESTINATION "share/doc/${component}"
          COMPONENT ${component}
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
          )
endforeach()

#Rename every package
foreach(comp ${COMPONENTS_LIST})
  string(TOUPPER ${comp} COMP_UP)
  if(PACKAGING_RPM)
    set(CPACK_RPM_${COMP_UP}_PACKAGE_NAME ${comp})
    set(CPACK_RPM_${COMP_UP}_FILE_NAME "${comp}-${CPACK_PACKAGE_VERSION}-${OAI_SOVERSION}.${CMAKE_SYSTEM_PROCESSOR}.rpm")
    set(CPACK_RPM_${COMP_UP}_DEBUGINFO_PACKAGE ON)
  else()
    set(CPACK_DEBIAN_${COMP_UP}_PACKAGE_NAME "${comp}")
    set(CPACK_DEBIAN_${COMP_UP}_FILE_NAME "${comp}.deb")
  endif()
endforeach()

INCLUDE(CPack)

