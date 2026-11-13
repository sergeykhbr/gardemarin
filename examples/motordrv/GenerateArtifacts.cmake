if(NOT EXISTS "${ELF_FILE}")
    message(FATAL_ERROR "ELF file not found: ${ELF_FILE}")
endif()

set(DUMP_FILE "${OUTPUT_DIR}/motordrv.dump")
set(LST_FILE  "${OUTPUT_DIR}/motordrv.lst")
set(BIN_FILE  "${OUTPUT_DIR}/motordrv.bin")

execute_process(
    COMMAND "${OBJDUMP}"
        --disassemble-all
        --disassemble-zeroes
        --section=.text
        --section=.text.isr_vector
        --section=.data
        "${ELF_FILE}"
    OUTPUT_FILE "${DUMP_FILE}"
    RESULT_VARIABLE RESULT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "objdump dump generation failed: ${RESULT}")
endif()

execute_process(
    COMMAND "${OBJDUMP}"
        -S
        "${ELF_FILE}"

    OUTPUT_FILE "${LST_FILE}"
    RESULT_VARIABLE RESULT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "objdump listing generation failed: ${RESULT}")
endif()

execute_process(
    COMMAND "${OBJCOPY}"
        -O binary
        "${ELF_FILE}"
        "${BIN_FILE}"

    RESULT_VARIABLE RESULT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "objcopy binary generation failed: ${RESULT}")
endif()

message(STATUS "ELF artifacts generated successfully.")
