#include <rtl-sdr.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cerrno>
#include <cstring>
#include <time.h>
//#include <libusb.h>

int main(int argc, char** argv) {
	if(argc > 1) {
		fprintf(stderr, "%s: error: unknown parameter \'%s\'\n", argv[0], argv[1]);
		return EXIT_FAILURE;
	}

	uint32_t device_count = rtlsdr_get_device_count();

	if(device_count < 1) {
		fprintf(stderr, "%s: error: rtlsdr_get_device_count() returned %d\n", argv[0], (int)device_count);
		return EXIT_FAILURE;
	}

	printf("%s: found %d device(s)\n", argv[0], device_count);
	fflush(stdout);

	uint32_t device_id = 0;

	printf("%s: selecting device %d:\n", argv[0], device_count);
	fflush(stdout);

	char* device_strings = (char*)malloc(sizeof(char)*(size_t)256*(size_t)3u);

	if(device_strings == NULL) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("error");
		return errno;
	}

	char* device_vendor  = &(device_strings[0*256]);
	char* device_product = &(device_strings[1*256]);
	char* device_serial  = &(device_strings[2*256]);

	errno = rtlsdr_get_device_usb_strings(device_id, device_strings+sizeof(char)*0*256, device_strings+sizeof(char)*1*256, device_strings+sizeof(char)*2*256);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_get_device_usb_strings() returned %d\n", argv[0], errno);
		free(device_strings);
		return errno;
	}

	printf(
	"%s:     vendor : %s\n"
	"%s:     product: %s\n"
	"%s:     serial : %s\n",
	argv[0], device_vendor,
	argv[0], device_product,
	argv[0], device_serial);
	fflush(stdout);

	free(device_strings);

	rtlsdr_dev_t* device;

	errno = rtlsdr_open(&device, device_id);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_open() returned error %d\n", argv[0], errno);
		return errno;
	}

	printf("%s: xtal frequencies:\n", argv[0]);
	fflush(stdout);

	uint32_t rtl_freq, tuner_freq;

	errno = rtlsdr_get_xtal_freq(device, &rtl_freq, &tuner_freq);

	if(errno != 0) {
		fprintf(stderr, "%s: warning: rtlsdr_get_xtal_freq() returned error %d\n", argv[0], errno);
	}

	printf(
	"%s:     rtl  : %u Hz\n"
	"%s:     tuner: %u Hz\n",
	argv[0], rtl_freq,
	argv[0], tuner_freq);
	fflush(stdout);

	//uint32_t center_freq = 100000000;
	uint32_t center_freq = 1575420000;

	printf("%s: tuning to %u Hz\n", argv[0], center_freq);
	fflush(stdout);

	errno = rtlsdr_set_center_freq(device, center_freq);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_set_center_freq() returned error %d\n", argv[0], errno);
		return errno;
	}

	printf("%s: tuner type: ", argv[0]);

	enum rtlsdr_tuner device_tuner_type = rtlsdr_get_tuner_type(device);

	switch(device_tuner_type) {
		case RTLSDR_TUNER_UNKNOWN:
			printf("unknown\n");
			fprintf(stderr, "%s: warning: rtlsdr_get_tuner_type() returned RTLSDR_TUNER_UNKNOWN\n", argv[0]);
			break;
		case RTLSDR_TUNER_E4000:
			printf("E4000\n");
			fprintf(stderr, "%s: warning: RTL-SDR-GNSS does not currently implement E4000 Tuner IF Gain\n", argv[0]);
			break;
		case RTLSDR_TUNER_FC0012:
			printf("FC0012\n");
			break;
		case RTLSDR_TUNER_FC0013:
			printf("FC0013\n");
			break;
		case RTLSDR_TUNER_FC2580:
			printf("FC2580\n");
			break;
		case RTLSDR_TUNER_R820T:
			printf("R820T/R820T2/R860\n");
			break;
		case RTLSDR_TUNER_R828D:
			printf("R828D\n");
			break;
		default:
			printf("unknown\n");
			fprintf(stderr, "%s: warning: rtlsdr_get_tuner_type() returned unrecognized tuner type %d\n", argv[0], device_tuner_type);
			break;
	}

	fflush(stdout);

	errno = rtlsdr_set_tuner_gain_mode(device, 1); // manual gain

	if(errno != 0) {
		fprintf(stderr, "%s: error: failed to set manual gain mode, rtlsdr_set_tuner_gain_mode() returned %d\n", argv[0], errno);
		return errno;
	}

	int num_tuner_gains = rtlsdr_get_tuner_gains(device, NULL);

	if(num_tuner_gains <= 0) {
		fprintf(stderr, "%s: error: failed to get number of available gain values, rtlsdr_get_tuner_gains() returned %d\n", argv[0], num_tuner_gains);
		return errno;
	}

	int* tuner_gains = (int*)malloc(sizeof(int)*(size_t)num_tuner_gains);

	if(tuner_gains == NULL) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("error");
		return errno;
	}

	errno = rtlsdr_get_tuner_gains(device, tuner_gains);

	if(num_tuner_gains <= 0) {
		fprintf(stderr, "%s: error: failed to get list of available gain values, rtlsdr_get_tuner_gains() returned %d\n", argv[0], num_tuner_gains);
		return errno;
	}

	printf("%s: available tuner gain value(s): ", argv[0]);

	for(int i = 0; i < (num_tuner_gains-1); i++) {
		printf("%d ", tuner_gains[i]);
	}

	printf("%d\n", tuner_gains[num_tuner_gains-1]);

	uint32_t tuner_gain = (uint32_t)386;

	printf("%s: setting tuner gain to %d\n", argv[0], tuner_gain);
	fflush(stdout);

	errno = rtlsdr_set_tuner_gain(device, tuner_gain);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_set_tuner_gain() returned error %d\n", argv[0], errno);
		return errno;
	}

	uint32_t sample_rate = 3200000;

	printf("%s: setting sample rate to %d\n", argv[0], sample_rate);
	fflush(stdout);

	errno = rtlsdr_set_sample_rate(device, sample_rate);

	if(errno == -EINVAL) {
		fprintf(stderr, "%s: error: failed to set sample rate, sample rate is invalid %d\n", argv[0], errno);
		return errno;
	}

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_set_sample_rate() returned error %d\n", argv[0], errno);
		return errno;
	}

	errno = rtlsdr_set_agc_mode(device, 0); // disable digital AGC if it isn't already

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_set_agc_mode() returned error %d\n", argv[0], errno);
		return errno;
	}

	int bias_tee = 1; // 0: off, 1: on

	errno = rtlsdr_set_bias_tee(device, bias_tee);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_set_center_freq() returned error %d\n", argv[0], errno);
		return errno;
	}

	int buffer_length = 262144, num_read;

	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char)*(size_t)2*(size_t)buffer_length);

	memset(buffer, 0, 2*buffer_length);

	if(buffer == NULL) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("error");
		return errno;
	}

	errno = rtlsdr_reset_buffer(device);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_reset_buffer() returned error %d\n", argv[0], errno);
		return errno;
	}

	struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};

	nanosleep(&ts, NULL);

	FILE* file = fopen("./data.bin", "wb");

	if(file == NULL) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("error");
		return errno;
	}

	for(int i = 0; i < 500; i++) {
		//errno = rtlsdr_reset_buffer(device);
		errno = rtlsdr_read_sync(device, (void*)buffer, buffer_length, &num_read);

		if(errno != 0) {
			fprintf(stderr, "%s: error: rtlsdr_read_sync() returned error %d\n", argv[0], errno);
			return errno;
		}

		if(num_read != buffer_length) {
			fprintf(stderr, "%s: error: buffer wasn't filled! %d\n", argv[0], errno);
			return errno;
		}

		printf("%f\n", 0.0078431372549019607843137254901960784313725490196078431372549019607843137254901960784313725490196078f*((float)buffer[16]-127.5));

		fwrite(buffer, sizeof(unsigned char), buffer_length, file);
	}

	fclose(file);

	free(buffer);
	free(tuner_gains);

	errno = rtlsdr_close(device);

	if(errno != 0) {
		fprintf(stderr, "%s: error: rtlsdr_close() returned error %d\n", argv[0], errno);
		return errno;
	}

	return EXIT_SUCCESS;
}
