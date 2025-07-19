use std::io::{self, Write};
use std::thread;
use std::time::Duration;
use rand::Rng;

const COLS: usize = 80;
const ROWS: usize = 24;
const DELAY: u64 = 80000;

fn main() -> io::Result<()> {
    let mut rng = rand::thread_rng();
    
    // Initialize matrix
    let mut matrix = [[' '; COLS]; ROWS];
    let mut column_start = [0; COLS];
    let mut column_length = [0; COLS];
    
    // Random initial values
    for j in 0..COLS {
        column_start[j] = -(rng.gen::<i32>() % (ROWS as i32 * 2));
        column_length[j] = rng.gen_range(5..15);
    }
    
    // Hide cursor and clear screen
    print!("\x1B[?25l\x1B[2J");
    io::stdout().flush()?;
    
    // Handle Ctrl+C to restore cursor
    ctrlc::set_handler(move || {
        print!("\x1B[?25h");
        std::process::exit(0);
    }).expect("Error setting Ctrl-C handler");
    
    loop {
        print!("\x1B[H"); // Move cursor to home position
        
        // Update columns
        for j in 0..COLS {
            if column_start[j] > (ROWS as i32 + column_length[j] as i32) {
                if rng.gen_ratio(1, 20) {
                    column_start[j] = -(rng.gen::<i32>() % (ROWS as i32 * 2));
                    column_length[j] = rng.gen_range(5..15);
                }
            }
            
            if rng.gen_ratio(1, 3) && column_start[j] >= 0 {
                // Sometimes skip update
            } else {
                column_start[j] += 1;
            }
            
            // Clear position behind the column
            let clear_pos = column_start[j] - column_length[j] as i32 - 1;
            if clear_pos >= 0 && clear_pos < ROWS as i32 {
                matrix[clear_pos as usize][j] = ' ';
            }
            
            // Fill new positions
            let start = column_start[j];
            for i in start..start + column_length[j] as i32 {
                if i >= 0 && i < ROWS as i32 {
                    matrix[i as usize][j] = if rng.gen_ratio(6, 10) {
                        rng.gen_range(b'0'..=b'9') as char
                    } else {
                        rng.gen_range(b'A'..=b'Z') as char
                    };
                }
            }
        }
        
        // Display matrix
        for i in 0..ROWS {
            for j in 0..COLS {
                if matrix[i][j] != ' ' {
                    // Calculate brightness
                    let mut brightness = 0;
                    for k in 0..3 {
                        if i >= k && matrix[i - k][j] != ' ' {
                            brightness += 1;
                        }
                    }
                    
                    // Color based on brightness
                    match brightness {
                        1 => print!("\x1B[97m{}\x1B[0m", matrix[i][j]), // White
                        2 => print!("\x1B[92m{}\x1B[0m", matrix[i][j]), // Bright green
                        _ => print!("\x1B[32m{}\x1B[0m", matrix[i][j]), // Green
                    }
                    
                    // Randomly clear some characters
                    if rng.gen_ratio(1, 8) {
                        matrix[i][j] = ' ';
                    }
                } else {
                    print!(" ");
                }
            }
            println!();
        }
        
        io::stdout().flush()?;
        thread::sleep(Duration::from_micros(DELAY));
    }
}