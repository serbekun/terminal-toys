use reqwest;
use serde_json::{Value, Error as JsonError};
use std::collections::HashMap;
use std::io;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Get username input
    println!("Enter GitHub username:");
    let mut username = String::new();
    io::stdin().read_line(&mut username)?;
    let username = username.trim();

    // Fetch repositories
    let url = format!("https://api.github.com/users/{}/repos", username);
    let client = reqwest::Client::new();
    let response = client
        .get(&url)
        .header("User-Agent", "Rust GitHub Language Analyzer")
        .send()
        .await?;

    if !response.status().is_success() {
        eprintln!("Error fetching data or user not found");
        return Ok(());
    }

    let repos: Vec<Value> = response.json().await?;

    // Collect language statistics
    let mut language_counts: HashMap<String, u32> = HashMap::new();

    for repo in repos {
        if let Some(lang) = repo["language"].as_str() {
            *language_counts.entry(lang.to_string()).or_insert(0) += 1;
        }
    }

    // Convert to vector and sort
    let mut languages: Vec<_> = language_counts.into_iter().collect();
    languages.sort_by(|a, b| b.1.cmp(&a.1));
use reqwest;
use serde_json::{Value, Error as JsonError};
use std::collections::HashMap;
use std::io;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Get username input
    println!("Enter GitHub username:");
    let mut username = String::new();
    io::stdin().read_line(&mut username)?;
    let username = username.trim();

    // Fetch repositories
    let url = format!("https://api.github.com/users/{}/repos", username);
    let client = reqwest::Client::new();
    let response = client
        .get(&url)
        .header("User-Agent", "Rust GitHub Language Analyzer")
        .send()
        .await?;

    if !response.status().is_success() {
        eprintln!("Error fetching data or user not found");
        return Ok(());
    }

    let repos: Vec<Value> = response.json().await?;

    // Collect language statistics
    let mut language_counts: HashMap<String, u32> = HashMap::new();

    for repo in repos {
        if let Some(lang) = repo["language"].as_str() {
            *language_counts.entry(lang.to_string()).or_insert(0) += 1;
        }
    }

    // Convert to vector and sort
    let mut languages: Vec<_> = language_counts.into_iter().collect();
    languages.sort_by(|a, b| b.1.cmp(&a.1));

    // Print results
    println!("\nTop programming languages for {}:", username);
    println!("---------------------------------");
    for (i, (lang, count)) in languages.iter().take(10).enumerate() {
        println!("{}. {}: {} repositories", i + 1, lang, count);
    }

    Ok(())
}
    // Print results
    println!("\nTop programming languages for {}:", username);
    println!("---------------------------------");
    for (i, (lang, count)) in languages.iter().take(10).enumerate() {
        println!("{}. {}: {} repositories", i + 1, lang, count);
    }

    Ok(())
}