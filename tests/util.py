import subprocess
from constants import *
import segno
from segno import consts


def generate_bmp(executable: str, input: bytes):
    with open(TEMP_DATA_FILE, "wb") as f:
        f.write(input)

    generator_process = subprocess.run(
        [executable, "", TEMP_DATA_FILE], capture_output=True, check=True
    )
    bmp = generator_process.stdout

    with open(TEMP_BMP_FILE, "wb") as f:
        f.write(bmp)


def make():
    make_process = subprocess.run(["make", "-C", PROJECT_ROOT], capture_output=True)
    ret_code = make_process.returncode
    if ret_code != 0:
        print(f"Make failed with a return code of {ret_code}")
        print(f"Stdout:\n{make_process.stdout}")
        print(f"Stderr:\n{make_process.stderr}")
    else:
        print("Sucessfully compiled executables")


def get_encoding(data):
    return (
        "alphanumeric"
        if all([c in consts.ALPHANUMERIC_CHARS for c in data])
        else "bytes"
    )


def print_data_diffs(data, segno_data, forced_bytes=[]):
    diffs = 0
    bad_bytes = forced_bytes
    for i in range(min(len(data), len(segno_data))):
        a, b = data[i], segno_data[i]
        if a != b:
            print(f"Bit {i}, Byte {i//8}, program: {a}, segno: {b}")
            diffs += 1
            if i // 8 not in bad_bytes:
                bad_bytes.append(i // 8)
    for byte in bad_bytes:
        b1 = "".join([str(b) for b in data[byte * 8 : (byte + 1) * 8]])
        b2 = "".join([str(b) for b in segno_data[byte * 8 : (byte + 1) * 8]])
        print(f"Byte {byte}: program: {b1}, segno: {b2}")
    print(f"{diffs} total differences in bits")
    print(f"{len(bad_bytes)} total differences in bytes")
    print(f"Program generated length: {len(data)}")
    print(f"Segno generated length: {len(segno_data)}")


def segno_make_qr_replicated(
    content,
    error=None,
    version=None,
    mode=None,
    mask=None,
    encoding=None,
    eci=False,
    micro=None,
    boost_error=True,
    return_data=False,
    return_final_codewords=False,
):
    version = segno.encoder.normalize_version(version)
    if not micro and micro is not None and version in consts.MICRO_VERSIONS:
        raise ValueError(
            'A Micro QR Code version ("{0}") is provided but '
            'parameter "micro" is False'.format(segno.encoder.get_version_name(version))
        )
    if micro and version is not None and version not in consts.MICRO_VERSIONS:
        raise ValueError(
            'Illegal Micro QR Code version "{0}"'.format(
                segno.encoder.get_version_name(version)
            )
        )
    error = segno.encoder.normalize_errorlevel(error, accept_none=True)
    mode = segno.encoder.normalize_mode(mode)
    if (
        mode is not None
        and version is not None
        and not segno.encoder.is_mode_supported(mode, version)
    ):
        raise ValueError(
            'Mode "{0}" is not available in version "{1}"'.format(
                segno.encoder.get_mode_name(mode),
                segno.encoder.get_version_name(version),
            )
        )
    if error == consts.ERROR_LEVEL_H and (micro or version in consts.MICRO_VERSIONS):
        raise ValueError(
            'Error correction level "H" is not available for Micro QR Codes'
        )
    if eci and (micro or version in consts.MICRO_VERSIONS):
        raise ValueError("The ECI mode is not available for Micro QR Codes")
    segments = segno.encoder.prepare_data(content, mode, encoding)
    guessed_version = segno.encoder.find_version(segments, error, eci=eci, micro=micro)
    if version is None:
        version = guessed_version
    elif guessed_version > version:
        raise segno.encoder.DataOverflowError(
            'The provided data does not fit into version "{0}". Proposal: version {1}'.format(
                segno.encoder.get_version_name(version),
                segno.encoder.get_version_name(guessed_version),
            )
        )
    if error is None and version != consts.VERSION_M1:
        error = consts.ERROR_LEVEL_L
    is_micro = version < 1
    mask = segno.encoder.normalize_mask(mask, is_micro)
    return segno_encode(
        segments,
        error,
        version,
        mask,
        eci,
        boost_error,
        return_data,
        return_final_codewords,
    )


def segno_encode(
    segments,
    error,
    version,
    mask,
    eci,
    boost_error,
    return_data,
    return_final_codewords,
    sa_info=None,
):
    is_micro = version < 1
    sa_mode = sa_info is not None
    buff = segno.encoder.Buffer()
    ver = version
    ver_range = version
    if not is_micro:
        ver = None
        ver_range = segno.encoder.version_range(version)
    if boost_error:
        error = segno.encoder.boost_error_level(
            version, error, segments, eci, is_sa=sa_mode
        )
    if sa_mode:
        # ISO/IEC 18004:2015(E) -- 8 Structured Append (page 59)
        for i in sa_info[:3]:
            buff.append_bits(i, 4)
        buff.append_bits(sa_info.parity, 8)
    # ISO/IEC 18004:2015(E) -- 7.4 Data encoding (page 22)
    for segment in segments:
        segno.encoder.write_segment(buff, segment, ver, ver_range, eci)
    capacity = consts.SYMBOL_CAPACITY[version][error]
    # ISO/IEC 18004:2015(E) -- 7.4.9 Terminator (page 32)
    segno.encoder.write_terminator(buff, capacity, ver, len(buff))
    # ISO/IEC 18004:2015(E) -- 7.4.10 Bit stream to codeword conversion (page 34)
    segno.encoder.write_padding_bits(buff, version, len(buff))
    # ISO/IEC 18004:2015(E) -- 7.4.10 Bit stream to codeword conversion (page 34)
    segno.encoder.write_pad_codewords(buff, version, capacity, len(buff))
    if return_data:
        return buff.getbits()

    buff = segno.encoder.make_final_message(version, error, buff)
    if return_final_codewords:
        return buff.getbits()

    # Matrix with timing pattern and reserved format / version regions
    width = segno.encoder.calc_matrix_size(version)
    height = width
    matrix = segno.encoder.make_matrix(width, height)
    # ISO/IEC 18004:2015 -- 6.3.3 Finder pattern (page 16)
    segno.encoder.add_finder_patterns(matrix, width, height)
    # ISO/IEC 18004:2015 -- 6.3.6 Alignment patterns (page 17)
    segno.encoder.add_alignment_patterns(matrix, width, height)
    # ISO/IEC 18004:2015 -- 7.7 Codeword placement in matrix (page 46)
    segno.encoder.add_codewords(matrix, buff, version)
    # ISO/IEC 18004:2015(E) -- 7.8.2 Data mask patterns (page 50)
    # ISO/IEC 18004:2015(E) -- 7.8.3 Evaluation of data masking results (page 53)
    mask, matrix = segno.encoder.find_and_apply_best_mask(matrix, width, height, mask)
    # ISO/IEC 18004:2015(E) -- 7.9 Format information (page 55)
    segno.encoder.add_format_info(matrix, version, error, mask)
    # ISO/IEC 18004:2015(E) -- 7.10 Version information (page 58)
    segno.encoder.add_version_info(matrix, version)
    return segno.encoder.Code(matrix, version, error, mask, segments)
