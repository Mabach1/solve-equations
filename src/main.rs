use matrix::Matrix;
mod matrix;

fn main() {
    let m = Matrix::from_equations("1x-1y=3.5/-3x+8y=22");
    m.print();
}
