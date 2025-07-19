use std::io;

const INPUT_SIZE: usize = 4;
const FIRST_ADD: i32 = 7;
const SECOND_ADD: i32 = 1443;

fn main() {
    println!("Enter your age: ");
    
    let mut input = String::new();
    io::stdin()
        .read_line(&mut input)
        .expect("Failed to read input");
    
    // Trim whitespace and truncate to INPUT_SIZE
    let input = input.trim();
    let input = if input.len() > INPUT_SIZE {
        &input[..INPUT_SIZE]
    } else {
        input
    };
    
    let age: i32 = input.parse().unwrap_or(0);
    let age = age * FIRST_ADD;
    let age = age * SECOND_ADD;
    
    println!("{}", age);
}