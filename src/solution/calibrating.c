#include "calibrating.h"

#include <string.h>

#include "../logging.h"
#include "../comp/all.h"
#include "../my_hardware/timer.h"

typedef struct {
	double Sx, Sx2, Sx3, Sx4, Sy, Sxy, Sx2y;
} quad_reg_t;

typedef struct {
	quad_reg_t sol;
	uint32_t n;
} quad_reg_lin_t;

void clear_reg(quad_reg_t *reg) {
	reg->Sx   = 0;
	reg->Sx2  = 0;
	reg->Sx3  = 0;
	reg->Sx4  = 0;
	reg->Sy   = 0;
	reg->Sxy  = 0;
	reg->Sx2y = 0;
}
void add_elem_exp(quad_reg_t *reg, double x, double y, double smooth) {
	reg->Sx   = smooth * reg->Sx   + (1 - smooth) * x;
	reg->Sx2  = smooth * reg->Sx2  + (1 - smooth) * x * x;
	reg->Sx3  = smooth * reg->Sx3  + (1 - smooth) * x * x * x;
	reg->Sx4  = smooth * reg->Sx4  + (1 - smooth) * x * x * x * x;
	reg->Sy   = smooth * reg->Sy   + (1 - smooth) * y;
	reg->Sxy  = smooth * reg->Sxy  + (1 - smooth) * x * y;
	reg->Sx2y = smooth * reg->Sx2y + (1 - smooth) * x * x * y;
}
void add_elem_lin(quad_reg_lin_t *reg, double x, double y) {	
	reg->sol.Sx   += x;
	reg->sol.Sx2  += x * x;
	reg->sol.Sx3  += x * x * x;
	reg->sol.Sx4  += x * x * x * x;
	reg->sol.Sy   += y;
	reg->sol.Sxy  += x * y;
	reg->sol.Sx2y += x * x * y;
	reg->n++;
}

void solve_reg_exp(quad_reg_t *reg, double *a, double *b, double *c) {
	double D =  reg->Sx4 * (reg->Sx2           - reg->Sx  * reg->Sx) -
				reg->Sx3 * (reg->Sx3           - reg->Sx  * reg->Sx2) +
				reg->Sx2 * (reg->Sx3 * reg->Sx - reg->Sx2 * reg->Sx2);
	double Da = reg->Sx2y* (reg->Sx2           - reg->Sx  * reg->Sx) -
				reg->Sx3 * (reg->Sxy           - reg->Sx  * reg->Sy) +
				reg->Sx2 * (reg->Sxy * reg->Sx - reg->Sx2 * reg->Sy);
	double Db = reg->Sx4 * (reg->Sxy           - reg->Sx  * reg->Sy) -
				reg->Sx2y* (reg->Sx3           - reg->Sx  * reg->Sx2) +
				reg->Sx2 * (reg->Sx3 * reg->Sy - reg->Sxy * reg->Sx2);
	double Dc = reg->Sx4 * (reg->Sx2 * reg->Sy - reg->Sx  * reg->Sxy) -
				reg->Sx3 * (reg->Sx3 * reg->Sy - reg->Sx2 * reg->Sxy) +
				reg->Sx2y* (reg->Sx3 * reg->Sx - reg->Sx2 * reg->Sx2);
	*a = Da / D;
	*b = Db / D;
	*c = Dc / D;
}
void solve_reg_lin(quad_reg_lin_t *reg, double *a, double *b, double *c) {
	quad_reg_t copy = reg->sol;
	copy.Sx   /= reg->n;
	copy.Sx2  /= reg->n;
	copy.Sx3  /= reg->n;
	copy.Sx4  /= reg->n;
	copy.Sy   /= reg->n;
	copy.Sxy  /= reg->n;
	copy.Sx2y /= reg->n;
	solve_reg_exp(&copy, a, b, c);
}

#define LOOP_TIME 10000000
#define POLL_RATE 10000

uint32_t last_point = 0;
uint32_t last_show = 0;

quad_reg_lin_t reg;

float speed, read_speed = 0;
double a, b, c;

float triangle(float gen) {
	return (gen > 0.5 ? 2 - 2 * gen : 2 * gen) * (1 - 0.5) + 0.5;
}

void calibrate_loop() {
	uint32_t time = us_count();
	if (last_point + POLL_RATE < time) {
		last_point += POLL_RATE;
		read_speed = get_speed(enc) * WHEEL_RADIUS;
		if (read_speed) {
			add_elem_lin(&reg, speed, read_speed);
			//add_elem_exp(&reg.sol, speed, read_speed, 0.999);
			//solve_reg_exp(&reg.sol, &a, &b, &c);

			//logcsv("%f,%f", speed, read_speed);
			//logcsv("%f,%f,%f,%u", a, b, c, reg.n);
			logcsv("%f,%f,%f,%f,%f,%u", speed, read_speed, a, b, c, reg.n);
		}
	}
	speed = triangle((float)(time % LOOP_TIME) / LOOP_TIME);
	set_duty(motor_l, speed);
	set_duty(motor_r, speed);
	if (last_show + LOOP_TIME <= time) {
		last_show += LOOP_TIME;
		if (last_show <= LOOP_TIME) {
			memset(&reg, 0x00, sizeof(quad_reg_lin_t));
		}
		else {
			solve_reg_lin(&reg, &a, &b, &c);
		}
	}
}
