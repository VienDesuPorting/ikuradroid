/* ONScripter-Droid dispatch glue: ViLE entry point -> ONScripter runtime.
 *
 * ViLE's main() probes the game directory with ONScripterProbe() before
 * claiming SDL; when an NScripter script is found, ONScripterRun() hands
 * the directory over to the embedded ONScripter-Droid runtime, which
 * brings up its own window/renderer under USE_SDL_RENDERER.
 */
#ifndef ONS_GLUE_H
#define ONS_GLUE_H

/* Returns true when the directory holds an NScripter game script
 * (nscript.dat / nscr_sec.dat / nscript.___ / 0.txt / 00.txt). */
bool ONScripterProbe(const char *dir);

/* Runs the ONScripter runtime on the given directory until the game
 * quits. The process cwd is expected to already point at the game
 * directory (the SDL bridge chdirs before main()); dir is used for
 * explicit archive/save paths and encoding detection.
 * Returns 0 on clean exit, negative on failure. */
int ONScripterRun(const char *dir);

#endif
