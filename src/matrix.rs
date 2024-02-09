#[derive(Clone)]
pub struct Matrix {
    data: Vec<f64>,
    rows: usize,
    cols: usize,
}

impl Matrix {
    pub fn new(rows: usize, cols: usize) -> Self {
        Self {
            data: vec![0.0; rows * cols],
            rows: rows,
            cols: cols,
        }
    }

    pub fn from_string(string: &str) -> Self {
        let rows = string.chars().filter(|&e| e == ';').count() + 1;
        let chopped: Vec<_> = string.split(';').collect();
        let data: Vec<f64> = chopped
            .iter()
            .flat_map(|e| e.split_whitespace().map(|s| s.parse::<f64>().unwrap()))
            .collect();

        Self {
            data: data.clone(),
            rows,
            cols: data.len() / rows,
        }
    }

    pub fn print(&self) {
        for row in 0..self.rows {
            for col in 0..self.cols {
                print!("{:?} ", self.data.get(row * self.cols + col).unwrap());
            }
            println!();
        }
    }

    pub fn from_equations(equations: &str) -> Self {
        let mut data = Vec::new();
        let mut rows = 0;

        let equations: Vec<_> = equations.split('/').collect();
        for equation in &equations {
            let numbers: Vec<f64> = equation
                .split('=')
                .flat_map(|elem| {
                    let filtered_chars: Vec<char> = elem
                        .chars()
                        .map(|x| if x.is_alphabetic() { ' ' } else { x })
                        .collect();
                    let numbers_str: String = filtered_chars.iter().collect();
                    numbers_str
                        .split_whitespace()
                        .flat_map(|elem| elem.parse::<f64>())
                        .collect::<Vec<f64>>()
                })
                .collect();

            rows += 1;
            data.push(numbers);
        }

        let data = data.concat();

        Self {
            data: data.clone(),
            rows,
            cols: data.len() / rows,
        }
    }

    pub fn solve(&self) -> Vec<f64> {
        let mut data = self.data.clone();

        let mut divisor_idx = 0;

        for row_idx in 0..self.rows {
            let mut current_row =
                (&data[row_idx * self.cols..row_idx * self.cols + self.cols]).to_vec();

            let divisor = *current_row.get(divisor_idx).unwrap();

            if divisor == 0.0 {
                todo!("swapping rows / matrix is irregular");
            }

            current_row = apply(&current_row, |e| e / divisor);

            let mut iteration_res = vec![];
            iteration_res.append(&mut current_row.to_vec());

            // "applying" the current row to the rest of the matrix
            for rem_row_idx in row_idx + 1..self.rows {
                let remaining_row =
                    (&data[rem_row_idx * self.cols..rem_row_idx * self.cols + self.cols]).to_vec();

                let divided_row = apply(&current_row, |e| {
                    e * remaining_row.get(divisor_idx).unwrap() * -1.0
                });

                let mut row_sum = apply_on_rows(&remaining_row, &divided_row, |a, b| a + b);

                iteration_res.append(&mut row_sum);
            }

            data.splice(row_idx * self.cols.., iteration_res);
            divisor_idx += 1;
        }

        println!("{:?}", data);

        vec![]
    }
}

fn apply<F>(row: &Vec<f64>, func: F) -> Vec<f64>
where
    F: Fn(f64) -> f64,
{
    row.iter().map(|e| func(*e)).collect()
}

fn apply_on_rows<F>(row_a: &Vec<f64>, row_b: &Vec<f64>, func: F) -> Vec<f64>
where
    F: Fn(f64, f64) -> f64,
{
    row_a
        .iter()
        .zip(row_b.iter())
        .map(|(a, b)| func(*a, *b))
        .collect()
}
