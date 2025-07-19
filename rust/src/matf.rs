use std::io::{self, Write};
use std::process::Command;
use std::time::{SystemTime, UNIX_EPOCH};
use libc::{ioctl, winsize, STDOUT_FILENO, TIOCGWINSZ};
use rand::Rng;

fn main() -> io::Result<()> {
    let mut rng = rand::thread_rng();
    let start_time = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs();
    let mut seed = start_time as u64;
    seed ^= seed << 21;
    seed ^= seed >> 35;
    seed ^= seed << 4;
    rand::rngs::StdRng::seed_from_u64(seed);
    
    let mut w = winsize {
        ws_row: 0,
        ws_col: 0,
        ws_xpixel: 0,
        ws_ypixel: 0,
    };
    
    unsafe {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &mut w);
    }
    
    let cols = w.ws_col as usize;
    let rows = w.ws_row as usize;
    const DELAY: u64 = 50000;
    
    if cols < 80 || rows < 24 {
        eprintln!("Terminal too small! Minimum 80x24 required.");
        return Ok(());
    }
    
    let mut matrix: Vec<Vec<char>> = vec![vec![' '; cols]; 
    matrix.resize(rows, vec![' '; cols]);
    
    let mut column_start: Vec<i32> = (0..cols)
        .map(|_| -(rng.gen::<i32>() % rows as i32))
        .collect();
    
    print!("\x1B[?25l"); // Hide cursor
    io::stdout().flush()?;
    Command::new("clear").status()?;
    
    ctrlc::set_handler(move || {
        print!("\x1B[?25h"); // Show cursor before exit
        std::process::exit(0);
    }).expect("Error setting Ctrl-C handler");
    
    loop {
        print!("\x1B[H"); // Move cursor to home position
        
        for j in 0..cols {
            if column_start[j] < rows as i32 {
                column_start[j] += 1;
            } else if rng.gen_ratio(1, 20) {
                column_start[j] = -(rng.gen::<i32>() % (rows as i32 / 2));
            }
            
            let seg_length = rng.gen_range(5..15);
            for i in 0..rows {
                let i_i32 = i as i32;
                if i_i32 >= column_start[j] && i_i32 < column_start[j] + seg_length {
                    matrix[i][j] = if rng.gen_bool(0.5) {
                        (rng.gen_range(0..10) + b'0'
                    } else {
                        (rng.gen_range(0..26) + b'A'
                    } as char;
                } else if i_i32 == column_start[j] + seg_length && rng.gen_ratio(1, 3) {
                    matrix[i][j] = ' ';
                }
            }
        }
        
        for i in 0..rows {
            for j in 0..cols {
                if matrix[i][j] != ' ' {
                    let mut bright = 0;
                    let trail = i < rows - 1;
                    
                    for k in 0..3 {
                        if i >= k && matrix[i - k][j] != ' ' {
                            bright += 1;
                        }
                    }
                    
                    match bright {
                        1 => print!("\x1B[38;2;200;200;255m{}\x1B[0m", matrix[i][j]),
                        2 => print!("\x1B[38;2;150;150;255m{}\x1B[0m", matrix[i][j]),
                        _ => print!("\x1B[38;2;50;50;200m{}\x1B[0m", matrix[i][j]),
                    }
                    
                    if trail && rng.gen_ratio(1, 8) {
                        matrix[i][j] = ' ';
                    }
                } else {
                    print!(" ");
                }
            }
        }
        
        io::stdout().flush()?;
        std::thread::sleep(std::time::Duration::from_micros(DELAY));
    }
}