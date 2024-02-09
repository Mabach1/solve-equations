pub mod matrix;

fn main() {
    let input = "1 1 0 3; 1 0 1 2; 0 1 1 1";
    let m = matrix::Matrix::from_string(input);
    m.solve();
}
