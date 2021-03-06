// See LICENSE for license details.

#ifndef _RISCV_SIM_H
#define _RISCV_SIM_H

#include <vector>
#include <string>
#include <memory>
#include "processor.h"
#include "mmu.h"
#include "devices.h"

class htif_isasim_t;

// this class encapsulates the processors and memory in a RISC-V machine.
class sim_t
{
public:
  sim_t(const char* isa, size_t _nprocs, size_t mem_mb,
        const std::vector<std::string>& htif_args);
  ~sim_t();

  // run the simulation to completion
  int run();
  bool running();
  void stop();
  void set_debug(bool value);
  void set_log(bool value);
  void set_histogram(bool value);
  void set_procs_debug(bool value);
  void set_memwordsize(uint32_t value) { memwordsize = value; } //MWG
  uint32_t get_memwordsize() { return memwordsize; } //MWG
  void enable_memdatatrace() { memdatatrace_en = true; } //MWG
  void disable_memdatatrace() { memdatatrace_en = false; } //MWG
  void set_memdatatrace_rand(bool value) { memdatatrace_rand = value; } //MWG
  inline bool memdatatrace_enabled() { return memdatatrace_en; } //MWG
  inline bool get_memdatatrace_rand() { return memdatatrace_rand; } //MWG
  void set_memdatatrace_step_begin(size_t value) { memdatatrace_step_begin = value; } //MWG
  void set_memdatatrace_step_end(size_t value) { memdatatrace_step_end = value; } //MWG
  void set_memdatatrace_sample_interval(size_t value) { memdatatrace_sample_interval = value; } //MWG
  void set_memdatatrace_rand_prob_recip(size_t value) { memdatatrace_rand_prob_recip = value; } //MWG
  inline size_t get_memdatatrace_step_begin() { return memdatatrace_step_begin; } //MWG
  inline size_t get_memdatatrace_step_end() { return memdatatrace_step_end; } //MWG
  inline size_t get_memdatatrace_sample_interval() { return memdatatrace_sample_interval; } //MWG
  inline size_t get_memdatatrace_rand_prob_recip() { return memdatatrace_rand_prob_recip; } //MWG
  htif_isasim_t* get_htif() { return htif.get(); }

  // returns the number of processors in this simulator
  size_t num_cores() { return procs.size(); }
  processor_t* get_core(size_t i) { return procs.at(i); }

  // read one of the system control registers
  reg_t get_scr(int which);

private:
  std::unique_ptr<htif_isasim_t> htif;
  char* mem; // main memory
  size_t memsz; // memory size in bytes
  mmu_t* debug_mmu;  // debug port into main memory
  std::vector<processor_t*> procs;
  std::unique_ptr<rom_device_t> devicetree;
  bus_t bus;

  processor_t* get_core(const std::string& i);
  void step(size_t n); // step through simulation
  static const size_t INTERLEAVE = 5000;
  static const size_t INSNS_PER_RTC_TICK = 100; // 10 MHz clock for 1 BIPS core
  reg_t rtc;
  size_t current_step;
  //size_t total_steps; //MWG
  size_t current_proc;
  bool debug;
  bool log;
  bool histogram_enabled; // provide a histogram of PCs
  bool memdatatrace_en; //MWG
  bool memdatatrace_rand; //MWG
  size_t memdatatrace_step_begin; //MWG
  size_t memdatatrace_step_end; //MWG
  size_t memdatatrace_sample_interval; //MWG
  size_t memdatatrace_rand_prob_recip; //MWG
  uint32_t memwordsize; //MWG 

  // memory-mapped I/O routines
  bool mmio_load(reg_t addr, size_t len, uint8_t* bytes);
  bool mmio_store(reg_t addr, size_t len, const uint8_t* bytes);
  void make_device_tree();

  // presents a prompt for introspection into the simulation
  void interactive();

  // functions that help implement interactive()
  void interactive_help(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_quit(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run(const std::string& cmd, const std::vector<std::string>& args, bool noisy);
  void interactive_run_noisy(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run_silent(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_reg(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregs(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregd(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_pc(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_mem(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_str(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_until(const std::string& cmd, const std::vector<std::string>& args);
  reg_t get_reg(const std::vector<std::string>& args);
  reg_t get_freg(const std::vector<std::string>& args);
  reg_t get_mem(const std::vector<std::string>& args);
  reg_t get_pc(const std::vector<std::string>& args);
  reg_t get_tohost(const std::vector<std::string>& args);

  friend class htif_isasim_t;
  friend class processor_t;
  friend class mmu_t;
  friend class cache_sim_t; //MWG
};

extern volatile bool ctrlc_pressed;

#endif
