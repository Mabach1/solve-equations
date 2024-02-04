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

}
