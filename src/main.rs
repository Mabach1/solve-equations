use matrix::Matrix;
mod matrix;

fn main() {
    let input = r#"2x-4y=6.4/-3x+8y=22/1x+10y=20"#;
    let m = Matrix::from_equations(input);
    m.solve();
}
