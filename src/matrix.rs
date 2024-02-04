#[derive(Clone)]
pub struct Matrix {
    data: Vec<f32>,
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
        let data: Vec<f32> = chopped
            .iter()
            .flat_map(|e| e.split_whitespace().map(|s| s.parse::<f32>().unwrap()))
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
            let numbers: Vec<f32> = equation
                .split('=')
                .flat_map(|elem| {
                    let filtered_chars: Vec<char> = elem
                        .chars()
                        .map(|x| if x.is_alphabetic() { ' ' } else { x })
                        .collect();
                    let numbers_str: String = filtered_chars.iter().collect();
                    numbers_str
                        .split_whitespace()
                        .flat_map(|elem| elem.parse::<f32>())
                        .collect::<Vec<f32>>()
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

    pub fn solve(&self) -> Vec<f32> {
        // index of values that will be 1
        let mut idx = 0;

        let mut fst_iteration_result: Vec<f32> = vec![];

        for row_idx in 0..self.rows {
            let current_row = (&self.data[row_idx * self.cols..row_idx * self.cols + self.cols]).to_vec();

            // the value of element we want to be 1
            let divisor = current_row.get(idx).unwrap().clone();
            let divided_row = apply(&current_row, |e| e / divisor);

            fst_iteration_result.append(&mut divided_row.clone());

            println!("current row {:?}", divided_row);

            for rest_row_idx in row_idx..self.rows - 1 {
                let row = (&self.data[rest_row_idx * self.cols..rest_row_idx * self.cols + self.cols]).to_vec();

                // modify rows                

                println!("remaining row {:?}", row);
            }

            idx += 1;
        }
        vec![]
    }
}

fn apply<F>(row: &Vec<f32>, func: F) -> Vec<f32>
where
    F: Fn(f32) -> f32,
{
    row.iter().map(|e| func(*e)).collect()
}

fn apply_on_rows<F>(row_a: &Vec<f32>, row_b: &Vec<f32>, func: F) -> Vec<f32>
where
    F: Fn(f32, f32) -> f32,
{
    row_a
        .iter()
        .zip(row_b.iter())
        .map(|(a, b)| func(*a, *b))
        .collect()
}
