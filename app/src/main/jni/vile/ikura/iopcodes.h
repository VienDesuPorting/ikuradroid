/*! \unit iopcodes.h
 *  \brief defines symbols for ikura op codes
 *
 *	Reversed engineered from the executables. Googletranslated comments.
 */
#ifndef _IKURAOPS_H_
#define _IKURAOPS_H_

#define IOP_EXT				0xFF	// Extended treatment
#define IOP_BREAK			0xFE	// Break
#define IOP_GRPOUT			0xFD	// Image Output
#define IOP_TIMERGET		0xFC	// Get Time Counter
#define IOP_TIMEREND		0xFB	// Exit Time Counter
#define IOP_TIMERSET		0xFA	// Set Time Counter
#define IOP_SETFONTCOLOR	0xF9	// Specifies the font color
#define IOP_SETFONTSTYLE	0xF8	// specify font style
#define IOP_SETGAMEINFO		0xF7	// Setting the in-game information
#define IOP_PPE				0xF6	// ban display settings allow pop-up menu
#define IOP_SVF				0xF5	// save set not to allow
#define IOP_PPF				0xF4	// Display Settings pop-up menu
#define IOP_AVIP			0xF3	// AVI file playback
#define IOP_AWAIT			0xF2	// Wait Wait
#define IOP_ATIMES			0xF1	// Starting weight
#define IOP_CNF				0xF0	// Set the file name bound
#define IOP_EXT2			0xEF	// Extended treatment
#define IOP_TITLE			0xE9	// Title Screen
#define IOP_ENABLE			0xE8	// enable Dialog Control
#define IOP_DISABLE			0xE7	// disable the Dialog
#define IOP_OPPROP			0xE6	// Open dialog
#define IOP_OPSL			0xE5	// SAVE/LOAD dialog open
#define IOP_SLDRSET			0xE4	// Settings dialog slider
#define IOP_CBSET			0xE3	// Settings dialog box, check
#define IOP_RBSET			0xE2	// Set the radio button of the dialog
#define IOP_FRAMESET		0xE1	// Settings dialog frame
#define IOP_TAGSET			0xE0	// tagging dialog
#define IOP_MPM2			0xD9	// Multi-view
#define IOP_PM2				0xD8	// character display
#define IOP_VOC				0xD7	// Voice File Name Registration
#define IOP_MPM				0xD6	// execute simultaneous multi-line display
#define IOP_SABL			0xD5	// --- no description ---
#define IOP_PPCRT			0xD4	// return to game to close pop-up menu
#define IOP_PPORT			0xD3	// return pop-up menu
#define IOP_PPTYPE			0xD2	// Setting pop-up menu display type
#define IOP_PPABL			0xD1	// enable disable pop-up menu configuration
#define IOP_PPTL			0xD0	// Set the popup menu title
#define IOP_IROPN			0xBF	// --- no description ---
#define IOP_IRCLK			0xBE	// --- no description ---
#define IOP_CLKEXMCSET		0xBD	// mouse cursor waiting ID extension init
#define IOP_SETSMPRATE		0xB4	// set sample rate
#define IOP_MSGBOX			0xB3	// Display Message Box
#define IOP_CHANGEWALL		0xB2	// wallpaper change
#define IOP_NAMECOPY		0xB1	// Copy Name
#define IOP_INNAME			0xB0	// hero name registration
#define IOP_KIDSCAN			0xA7	// determinate flag read and read features
#define IOP_KIDHABA			0xA6	// characters per line of text read
#define IOP_KIDFN			0xA5	// Set the number of read flags
#define IOP_KIDEND			0xA4	// read and sentences flagged as read
#define IOP_KIDSET			0xA3	// determinate flag read and sentences read
#define IOP_KIDPAGE			0xA2	// Read text pages of information
#define IOP_KIDMOJI			0xA1	// to set the text color of text read
#define IOP_KIDCLR			0xA0	// Initialize read and sentences
#define IOP_SETINSIDEVOL	0x9F	// internal volume setting
#define IOP_DAS				0x92	// CDDA Stop
#define IOP_DAP				0x91	// CDDA Playback
#define IOP_DAE				0x90	// CDDA Set
#define IOP_IGN				0x8E	// NO Get Cursor
#define IOP_CLK				0x8D	// Click wait
#define IOP_IHGP			0x8C	// Specify image transfer
#define IOP_IHGC			0x8B	// Select Reiautozerokuria
#define IOP_IHGL			0x8A	// Image loading layout selection
#define IOP_IHKDEF			0x89	// Keyboard Extension - Set default number
#define IOP_IHK				0x88	// Extended keyboard - the target data set
#define IOP_IGRELEASE		0x87	// mouse input in the screen - free
#define IOP_IGINIT			0x86	// mouse input in the screen - Init
#define IOP_IG				0x85	// mouse input in the screen
#define IOP_IH				0x84	// IG command set selection
#define IOP_IXY				0x83	// Changing the mouse position
#define IOP_IMS				0x82	// mouse movement range setting
#define IOP_IC				0x81	// Changing mouse cursor
#define IOP_IM				0x80	// Reading Mausukasorudeta
#define IOP_PCMCN			0x7F	// Backup audio file
#define IOP_PCMGETPOS		0x7E	// PCM playback position acquisition
#define IOP_SEGETPOS		0x7D	// Get sound effects playback position
#define IOP_BGMGETPOS		0x7C	// Get music playback position
#define IOP_SES				0x7B	// SES Stop Sound Effects
#define IOP_PCMEND			0x7A	// PCM Audio Stop waiting
#define IOP_PCMS			0x79	// PCM Stop
#define IOP_PCML			0x78	// PCM Load
#define IOP_PCMON			0x77	// PCM Audio Playback
#define IOP_SET				0x76	// Disable sound effects
#define IOP_SEP				0x75	// Enable sound effects
#define IOP_SER				0x74	// Loading sound effects
#define IOP_MS				0x73	// Stop the Music
#define IOP_MF				0x72	// Music fades
#define IOP_MP				0x71	// Musical Play
#define IOP_ML				0x70	// playing music, data loading
#define IOP_SGL				0x6F	// Load saved image
#define IOP_GADELETE		0x69	// Remove animation
#define IOP_GACLOSE			0x68	// animation file closed
#define IOP_GAPOS			0x67	// Animation Set position
#define IOP_GASET			0x66	// a set of animation data
#define IOP_GAOPEN			0x65	// Opening animation files
#define IOP_GAL				0x64	// Set animation loop
#define IOP_GV				0x63	// Screen shake treatment
#define IOP_GSCRL			0x62	// scroll handle
#define IOP_GPE				0x61	// Scale Processing
#define IOP_GGE				0x60	// effects using grayscale
#define IOP_GPO				0x5F	// BGM fade out effects and graphics
#define IOP_GPI				0x5E	// BGM fade effects and graphics
#define IOP_GASTOP			0x5D	// Stop Animation
#define IOP_GASTART			0x5C	// Start animation
#define IOP_PR				0x5B	// text display
#define IOP_GPJ				0x5A	// Character Set Form
#define IOP_GPB				0x59	// Set Text Size
#define IOP_GB				0x58	// Draw a rectangle
#define IOP_GP				0x57	// Copying graphics
#define IOP_GL				0x56	// Graphic Road Show
#define IOP_GO				0x55	// Gurafikkufedoauto
#define IOP_GI				0x54	// Gurafikkufedoin
#define IOP_GC				0x53	// Clear graphics
#define IOP_GF				0x52	// Graphic display off
#define IOP_GN				0x51	// On graphic display
#define IOP_VSET			0x50	// Virtual Set VRAM
#define IOP_SSP				0x4C	// copy parameters to system variables
#define IOP_SCP				0x4B	// Copy System Variables
#define IOP_EXC				0x4A	// EXA flags secured area
#define IOP_EXS				0x49	// EXA flags set in the command area
#define IOP_EXA				0x48	// extra space to store flags and vars
#define IOP_IF				0x47	// IF-THEN run
#define IOP_HT				0x46	// Transfer variable
#define IOP_HSG				0x45	// assign values to variables together
#define IOP_CALC			0x44	// Calculate
#define IOP_HDEC			0x43	// decrement a variable
#define IOP_HINC			0x42	// Increment a variable
#define IOP_HS				0x41	// assign a value to a variable
#define IOP_HLN				0x40	// set the number of variables
#define IOP_HXP				0x3C	// 2 jump determination flag pattern
#define IOP_HN				0x3B	// jump flag decision
#define IOP_STX				0x3A	// jump judging system flags
#define IOP_EC				0x39	// flag specified load
#define IOP_ES				0x38	// Saves the specified flags
#define IOP_STS				0x37	// Flag for Follow Up System
#define IOP_HP				0x36	// jump determination flag pattern
#define IOP_SP				0x35	// set flag pattern
#define IOP_FT				0x34	// Transfer flag
#define IOP_HF				0x33	// jump flag decision
#define IOP_SKS				0x32	// Set clear reversal flag together
#define IOP_SK				0x31	// reversal clear set of flags
#define IOP_FLN				0x30	// Setting the number of flags
#define IOP_WSS				0x2E	// the message window displays
#define IOP_WSH				0x2D	// Hidden Message Window
#define IOP_PMP				0x2C	// Check character display with audio flag
#define IOP_PM				0x2B	// character display
#define IOP_WC				0x2A	// Close Window
#define IOP_WO				0x29	// Open Window
#define IOP_PC				0x28	// Character Set Form
#define IOP_PB				0x27	// specify font size
#define IOP_PF				0x26	// Speed Setting Message Display
#define IOP_CNS				0x25	// Set figure name
#define IOP_CN				0x24	// character set name characters
#define IOP_WW				0x23	// Wait Click Settings
#define IOP_WL				0x22	// Wait, click Import Parts
#define IOP_WP				0x21	// Read the window parts
#define IOP_WS				0x20	// Set window position
#define IOP_CTEXT			0x1C	// Text Display
#define IOP_CRND			0x1B	// Random placement of the command
#define IOP_CRESET			0x1A	// Preparing for name of the command set
#define IOP_CCLR			0x19	// Clear the name of the command
#define IOP_CC				0x18	// Run command selection
#define IOP_CWC				0x17	// the Command window closed
#define IOP_CWO				0x16	// Open Command Window
#define IOP_CSET			0x15	// name of the command set
#define IOP_CIP				0x14	// Command Icon
#define IOP_CPS				0x13	// Setting the Character Palette
#define IOP_CIR				0x12	// Loading Icon
#define IOP_CP				0x11	// Read command window frames
#define IOP_CW				0x10	// Location of command window
#define IOP_URL				0x0A	// URL address of the call
#define IOP_CHILD			0x09	// child process
#define IOP_ONJS			0x08	// conditional subroutine
#define IOP_ONJP			0x07	// Jump conditions
#define IOP_RT				0x06	// return from subroutine
#define IOP_JS				0x05	// jump subroutine
#define IOP_JP				0x04	// jump
#define IOP_SRET			0x03	// Returning from a sub-scenarios
#define IOP_LSBS			0x02	// load and run sub-scenarios
#define IOP_LS				0x01	// load and run scenarios
#define IOP_ED				0x00	// end

// Aliased 
#define IOP_STC				IOP_STX	// jump judging system flags

// Undocumented
#define IOP_UDAE			0xAE
#define IOP_UDAF			0xAF
#define IOP_UDDA			0xDA

#endif

